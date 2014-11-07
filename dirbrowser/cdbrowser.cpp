/* Copyright (C) 2005 J.F.Dockes
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <fcntl.h>
#include <sys/stat.h>

#include <iostream>
using namespace std;

#include <QWebFrame>
#include <QWebSettings>
#include <QUrl>
#include <QWebElement>
#include <QMenu>

#include "HelperStructs/Helper.h"
#include "HelperStructs/CSettingsStorage.h"

#include "libupnpp/log.hxx"
#include "libupnpp/control/discovery.hxx"

#include "upqo/cdirectory_qo.h"
#include "upadapt/upputils.h"
#include "upadapt/md5.hxx"
#include "cdbrowser.h"
#include "rreaper.h"

using namespace UPnPClient;

static const string minimFoldersViewPrefix("0$folders");

static QByteArray readfile(const char *fn)
{
    int fd = -1;
    char *cp = 0;
    QByteArray ret;
    struct stat st;

    fd = open(fn, 0);
    if (fd < 0 || fstat(fd, &st) < 0) {
        goto out;
    }
    cp = (char *)malloc(st.st_size);
    if (cp == 0) {
        goto out;
    }
    if (read(fd, cp, st.st_size) != st.st_size) {
        goto out;
    }
    ret.append(cp, st.st_size);
out:
    if (fd >= 0)
        close(fd);
    if (cp)
        free(cp);
    return ret;
}

void CDBrowser::setStyleSheet(bool dark)
{
    QString cssfn = Helper::getSharePath() + "cdbrowser/cdbrowser.css";
    QByteArray css = readfile((const char *)cssfn.toLocal8Bit());

    if (dark) {
        cssfn = Helper::getSharePath() + "cdbrowser/dark.css";
        css += readfile((const char *)cssfn.toLocal8Bit());
    } else {
        cssfn = Helper::getSharePath() + "cdbrowser/standard.css";
    }
    css += readfile((const char *)cssfn.toLocal8Bit());

    css = QByteArray("data:text/css;charset=utf-8;base64,") +
        css.toBase64();

    QUrl cssurl(QString::fromUtf8((const char*)css));
    settings()->setUserStyleSheetUrl(cssurl);
}

CDBrowser::CDBrowser(QWidget* parent)
    : QWebView(parent), m_reader(0), m_reaper(0), m_insertactive(false)
{
    connect(this, SIGNAL(linkClicked(const QUrl &)), 
	    this, SLOT(onLinkClicked(const QUrl &)));

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);

    setStyleSheet(CSettingsStorage::getInstance()->getPlayerStyle());
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(createPopupMenu(const QPoint&)));
    connect(page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize&)),
            this, SLOT(onContentsSizeChanged(const QSize&)));

    setHtml("<html><head><title>Upplay directory browser !</title></head>"
            "<body><p>Looking for servers...</p>"
            "</body></html>");

    m_timer.setSingleShot(1);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(serversPage()));
    m_timer.start(0);
}

CDBrowser::~CDBrowser()
{
    delete m_reader;
}

void CDBrowser::onContentsSizeChanged(const QSize&)
{
    //qDebug() << "CDBrowser::onContentsSizeChanged: scrollpos " <<
    // page()->mainFrame()->scrollPosition();
    page()->mainFrame()->setScrollPosition(m_savedscrollpos);
}


void CDBrowser::appendHtml(const QString& elt_id, const QString& html)
{
    //LOGDEB("CDBrowser::appendHtml: " << qs2utf8s(html) << endl);
    
    QWebFrame *mainframe = page()->mainFrame();
    StringObj morehtml(html);

    mainframe->addToJavaScriptWindowObject("morehtml", &morehtml, 
                                           QScriptEngine::ScriptOwnership);
    QString js;
    if (elt_id.isEmpty()) {
        js = QString("document.body.innerHTML += morehtml.text");
    } else {
        js = QString("document.getElementById(\"%1\").innerHTML += morehtml.text").arg(elt_id);
    }
    mainframe->evaluateJavaScript(js);
}

void CDBrowser::onLinkClicked(const QUrl &url)
{
    m_timer.stop();
    string scurl = qs2utf8s(url.toString());
    //LOGDEB("CDBrowser::onLinkClicked: " << scurl << endl);

    int what = scurl[0];

    switch (what) {

    // Browse server root
    case 'S':
    {
	unsigned int cdsidx = atoi(scurl.c_str()+1);
        if (cdsidx > m_msdescs.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad link index: " << cdsidx 
                   << " cd count: " << m_msdescs.size() << endl);
	    return;
	}
        m_curpath.clear();
        m_curpath.push_back(CtPathElt("0", "(servers)"));
        m_ms = MSRH(new MediaServer(m_msdescs[cdsidx]));
        if (!m_ms) {
            qDebug() << "MediaServer connect failed";
            return;
        }
	browseContainer("0", m_msdescs[cdsidx].friendlyName);
    }
    break;

    case 'I':
    {
        // Item clicked add to playlist

	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_entries.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_entries.size() << endl);
	    return;
	}
        MetaDataList mdl;
        mdl.resize(1);
        udirentToMetadata(&m_entries[i], &mdl[0]);
        emit sig_tracks_to_playlist(mdl);
    }
    break;

    case 'C':
    {
        // Container clicked
        m_curpath.back().scrollpos = page()->mainFrame()->scrollPosition();
	unsigned int i = atoi(scurl.c_str()+1);
        if (i > m_entries.size()) {
	    LOGERR("CDBrowser::onLinkClicked: bad objid index: " << i 
                   << " id count: " << m_entries.size() << endl);
	    return;
	}
	browseContainer(m_entries[i].m_id, m_entries[i].m_title);
    }
    break;

    case 'L':
    {
        // Click in curpath section.
	unsigned int i = atoi(scurl.c_str()+1);
        curpathClicked(i);
        return;
    }
    break;

    default:
        LOGERR("CDBrowser::onLinkClicked: bad link type: " << what << endl);
        return;
    }
}

void CDBrowser::curpathClicked(unsigned int i)
{
    // qDebug() << "CDBrowser::curpathClicked: " << i << " pathsize " << 
    // m_curpath.size();
    if (i > m_curpath.size()) {
        LOGERR("CDBrowser::curPathClicked: bad curpath index: " << i 
               << " path count: " << m_curpath.size() << endl);
        return;
    }
    string objid = m_curpath[i].objid;
    string title = m_curpath[i].title;
    QPoint scrollpos = m_curpath[i].scrollpos;
    m_curpath.erase(m_curpath.begin()+i, m_curpath.end());
    if (i == 0) {
        m_msdescs.clear();
        serversPage();
    } else if (i == 1) {
        // Server root
        browseContainer("0", title, scrollpos);
    } else {
        browseContainer(objid, title, scrollpos);
    }
}

static const QString init_container_page(
    "<html><head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
    "</head><body>"
    "</body></html>"
    );

void CDBrowser::initContainerHtml()
{
    QString htmlpath("<div id=\"browsepath\"><ul>");
    for (unsigned i = 0; i < m_curpath.size(); i++) {
        QString title = QString::fromUtf8(m_curpath[i].title.c_str());
        QString objid = QString::fromUtf8(m_curpath[i].objid.c_str());
        htmlpath += 
            QString("<li class=\"container\" objid=\"%3\"><a href=\"L%1\">%2</a> &gt;</li>").
            arg(i).arg(title).arg(objid);
    }
    htmlpath += QString("</ul></div><br/>");

    setHtml(init_container_page);
    appendHtml(QString(), htmlpath);
    appendHtml(QString(), "<table id=\"entrylist\"></table>");
}

void CDBrowser::browseContainer(string ctid, string cttitle, QPoint scrollpos)
{
    qDebug() << "CDBrowser::browseContainer: " << " ctid " << ctid.c_str();
    m_savedscrollpos = scrollpos;
    if (!m_ms) {
        LOGERR("CDBrowser::browseContainer: server not set" << endl);
        return;
    }
    CDSH cds = m_ms->cds();
    if (!cds) {
        LOGERR("Cant reach content directory service" << endl);
        return;
    }
    m_entries.clear();

    m_curpath.push_back(CtPathElt(ctid, cttitle));

    initContainerHtml();

    if (m_reader) {
        delete m_reader;
        m_reader = 0;
    }
    m_reader = new ContentDirectoryQO(cds, ctid, this);

    connect(m_reader, SIGNAL(sliceAvailable(const UPnPClient::UPnPDirContent*)),
            this, SLOT(onSliceAvailable(const UPnPClient::UPnPDirContent *)));
    connect(m_reader, SIGNAL(done(int)), this, SLOT(onBrowseDone(int)));
    m_reader->start();
}


static QString CTToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    out += QString("<tr class=\"container\" objid=\"%1\" objidx=\"%2\"><td></td><td>").
        arg(e.m_id.c_str());
    out += QString("<a class=\"ct_title\" href=\"C%1\">").arg(idx);
    out += QString::fromUtf8(e.m_title.c_str());
    out += QString("</a></td></tr>");
    return out;
}

// Escape things that would look like HTML markup
static string escapeHtml(const string &in)
{
    string out;
    for (string::size_type pos = 0; pos < in.length(); pos++) {
	switch(in.at(pos)) {
	case '<': out += "&lt;"; break;
	case '>': out += "&gt;"; break;
	case '&': out += "&amp;"; break;
	case '"': out += "&quot;"; break;
	default: out += in.at(pos); break;
	}
    }
    return out;
}

/** @arg idx index in entries array
    @arg e array entry
*/
static QString ItemToHtml(unsigned int idx, const UPnPDirObject& e)
{
    QString out;
    string val;

    out = QString("<tr class=\"item\" objid=\"%1\" objidx=\"%2\">").
        arg(e.m_id.c_str()).arg(idx);

    e.getprop("upnp:originalTrackNumber", val);
    out += QString("<td class=\"tk_tracknum\">") + escapeHtml(val).c_str() + "</td>";

    out += "<td class=\"tk_title\">";
    out += QString("<a href=\"I%1\">").arg(idx);
    out += QString::fromUtf8(escapeHtml(e.m_title).c_str());
    out += "</a>";
    out += "</td>";

    val.clear();
    e.getprop("upnp:artist", val);
    out += "<td class=\"tk_artist\">";
    out += QString::fromUtf8(escapeHtml(val).c_str());
    out += "</td>";
    
    val.clear();
    e.getprop("upnp:album", val);
    out += "<td class=\"tk_album\">";
    out += QString::fromUtf8(escapeHtml(val).c_str());
    out += "</td>";
    
    val.clear();
    e.getrprop(0, "duration", val);
    int seconds = upnpdurationtos(val);
    // Format as mm:ss
    int mins = seconds / 60;
    int secs = seconds % 60;
    char sdur[100];
    sprintf(sdur, "%02d:%02d", mins, secs);
    out += "<td class=\"tk_duration\">";
    out += sdur;
    out += "</td>";

    out += "</tr>";

    return out;
}

void CDBrowser::onSliceAvailable(const UPnPDirContent *dc)
{
    QString html;

    // qDebug() << "CDBrowser::onSliceAvailable";
    m_entries.reserve(m_entries.size() + dc->m_containers.size() + 
                      dc->m_items.size());
    for (auto& entry: dc->m_containers) {
        // qDebug() << "Container: " << entry.dump().c_str();;
        m_entries.push_back(entry);
        html += CTToHtml(m_entries.size()-1, entry);
    }
    for (auto& entry: dc->m_items) {
        // qDebug() << "Item: " << entry.dump().c_str();;
        m_entries.push_back(entry);
        html += ItemToHtml(m_entries.size()-1, entry);
    }
    appendHtml("entrylist", html);
}

static bool dirObjCmpByURI(const UPnPDirObject& o1, const UPnPDirObject& o2)
{
    if (o1.m_resources.size() == 0 && o2.m_resources.size() == 0) {
        return o1.m_title < o2.m_title;
    } else if (o1.m_resources.size() == 0 && o2.m_resources.size() != 0) {
        return true;
    } else if (o1.m_resources.size() != 0 && o2.m_resources.size() == 0) {
        return false;
    } else {
        return o1.m_resources[0].m_uri < o2.m_resources[0].m_uri;
    }
}

void CDBrowser::onBrowseDone(int)
{
    //qDebug() <<"CDBrowser::onBrowseDone";
    if (m_reader) {
        ContentDirectory::ServiceKind kind = m_reader->getKind();
        if (kind == ContentDirectory::CDSKIND_MINIM && 
            CSettingsStorage::getInstance()->getFolderViewFNOrder() && 
            m_reader->getObjid().compare(0, minimFoldersViewPrefix.size(),
                                         minimFoldersViewPrefix) == 0) {
                
            sort(m_entries.begin(), m_entries.end(), dirObjCmpByURI);
            initContainerHtml();
            QString html;
            for (unsigned i = 0; i < m_entries.size(); i++) {
                if (m_entries[i].m_type == UPnPDirObject::container) {
                    html += CTToHtml(i, m_entries[i]);
                } else {
                    html += ItemToHtml(i, m_entries[i]);
                }
            }
            appendHtml("entrylist", html);
        }

        m_reader->wait();
        page()->mainFrame()->setScrollPosition(m_savedscrollpos);
    }
    
    //qDebug() << "CDBrowser::onBrowseDone done";
}


static const string init_server_page(
    "<html><head>"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
    "</head><body>"
    "<h2 id=\"cdstitle\">Content Directory Services</h2>"
    "</body></html>"
    );

static QString DSToHtml(unsigned int idx, const UPnPDeviceDesc& dev)
{
    QString out;
    out += QString("<p class=\"cdserver\" cdsid=\"%1\">").arg(idx);
    out += QString("<a href=\"S%1\">").arg(idx);
    out += QString::fromUtf8(dev.friendlyName.c_str());
    out += QString("</a></p>");
    return out;
}

void CDBrowser::serversPage()
{
    vector<UPnPDeviceDesc> msdescs;
    int secs = UPnPDeviceDirectory::getTheDir()->getRemainingDelay();
    if (secs > 1) {
        qDebug() << "CDBrowser::serversPage: waiting " << secs;
        m_timer.start(secs * 1000);
        return;
    }
    if (!MediaServer::getDeviceDescs(msdescs)) {
        LOGERR("CDBrowser::serversPage: getDeviceDescs failed" << endl);
        return;
    }
    //qDebug() << "CDBrowser::serversPage: " << msdescs.size() << " servers";
    
    bool same = msdescs.size() == m_msdescs.size();
    if (same) {
        for (unsigned i = 0; i < msdescs.size(); i++) {
            if (msdescs[i].UDN.compare(m_msdescs[i].UDN)) {
                same = false;
                break;
            }
        }
    }
    if (same) {
        //LOGDEB("CDBrowser::serversPage: no change" << endl);
        m_timer.start(5000);
        return;
    } else {
        //qDebug() << "CDBrowser::serversPage: updating";
    }
    m_msdescs = msdescs;
    setHtml(QString::fromUtf8(init_server_page.c_str()));
    for (unsigned i = 0; i < msdescs.size(); i++) {
        appendHtml("", DSToHtml(i, msdescs[i]));
    }
    m_timer.start(5000);
}

enum PopupMode {
    PUP_ADD,
    PUP_ADD_ALL,
    PUP_ADD_FROMHERE,
    PUP_BACK
};

void CDBrowser::createPopupMenu(const QPoint& pos)
{
    if (m_insertactive)
        return;
    qDebug() << "void CDBrowser::createPopupMenu(const QPoint& pos)";

    QWebHitTestResult htr = page()->mainFrame()->hitTestContent(pos);
    if (htr.isNull()) {
	return;
    }
    QWebElement el = htr.enclosingBlockElement();
    while (!el.isNull() && !el.hasAttribute("objid"))
	el = el.parent();

    if (el.isNull()) {
        // Click in blank space. We only have a Back action there for now
        if (m_curpath.size() == 0)
            return;
        QMenu *popup = new QMenu(this);
        QAction *act;
        QVariant v;
        act = new QAction(tr("Back"), this);
        v = QVariant(int(PUP_BACK));
        act->setData(v);
        popup->addAction(act);
        popup->connect(popup, SIGNAL(triggered(QAction *)), this, 
                       SLOT(back(QAction *)));
        popup->popup(mapToGlobal(pos));
	return;
    }

    // Clicked on some object. Dir entries inside the path have no objidx attr
    // and that's ok
    m_popupobjid = qs2utf8s(el.attribute("objid"));
    if (el.hasAttribute("objidx"))
        m_popupidx = el.attribute("objidx").toInt();
    else
        m_popupidx = -1;

    QString otype = el.attribute("class");
    qDebug() << "Popup: " << " class " << otype << " objid " << 
        m_popupobjid.c_str();

    if (m_popupidx == -1 && otype.compare("container")) {
        // All path elements should be containers !
        qDebug() << "Not container and no objidx??";
        return;
    }

    QMenu *popup = new QMenu(this);
    QAction *act;
    QVariant v;
    act = new QAction(tr("Send to playlist"), this);
    v = QVariant(int(PUP_ADD));
    act->setData(v);
    popup->addAction(act);

    if (!otype.compare("item")) {
        act = new QAction(tr("Send all to playlist"), this);
        v = QVariant(int(PUP_ADD_ALL));
        act->setData(v);
        popup->addAction(act);

        act = new QAction(tr("Send all from here to playlist"), this);
        v = QVariant(int(PUP_ADD_FROMHERE));
        act->setData(v);
        popup->addAction(act);
    }

    if (!otype.compare("container")) {
        popup->connect(popup, SIGNAL(triggered(QAction *)), this, 
                       SLOT(recursiveAdd(QAction *)));
    } else if (!otype.compare("item")) {
        popup->connect(popup, SIGNAL(triggered(QAction *)), this, 
                       SLOT(simpleAdd(QAction *)));
    } else {
        // ??
        qDebug() << "CDBrowser::createPopup: obj type neither ct nor it: " <<
            otype;
        return;
    }
    popup->popup(mapToGlobal(pos));
}

void CDBrowser::back(QAction *)
{
    if (m_curpath.size() >= 2)
        curpathClicked(m_curpath.size()-2);
}

// Add a single track or a section of the current container. This
// maybe triggered by a link click or a popup on an item entry
void CDBrowser::simpleAdd(QAction *act)
{
    //qDebug() << "CDBrowser::simpleAdd";
    m_popupmode = act->data().toInt();
    if (m_popupidx < 0 || m_popupidx > int(m_entries.size())) {
        LOGERR("CDBrowser::simpleAdd: bad obj index: " << m_popupidx
               << " id count: " << m_entries.size() << endl);
        return;
    }
    MetaDataList mdl;
    unsigned int starti = 0;
    switch (m_popupmode) {
    case PUP_ADD_FROMHERE: 
        starti = m_popupidx;
        /* FALLTHROUGH */
    case PUP_ADD_ALL:
        for (unsigned int i = 0; i < m_entries.size() - starti; i++) {
            unsigned int ei = starti + i;
            if (m_entries[ei].m_type == UPnPDirObject::item && 
                m_entries[ei].m_iclass == 
                UPnPDirObject::ITC_audioItem_musicTrack) {
                mdl.resize(mdl.size()+1);
                udirentToMetadata(&m_entries[ei], &mdl[i]);
            }
        }
        break;
    default:
        mdl.resize(1);
        udirentToMetadata(&m_entries[m_popupidx], &mdl[0]);
    }

    emit sig_tracks_to_playlist(mdl);
}

// Recursive add. This is triggered popup on a container
void CDBrowser::recursiveAdd(QAction *act)
{
    //qDebug() << "CDBrowser::recursiveAdd";
    m_insertactive = true;
    m_popupmode = act->data().toInt();

    if (!m_ms) {
        qDebug() << "CDBrowser::browseContainer: server not set" ;
        m_insertactive = false;
        return;
    }
    CDSH cds = m_ms->cds();
    if (!cds) {
        qDebug() << "Cant reach content directory service";
        m_insertactive = false;
        return;
    }
    ContentDirectory::ServiceKind kind = cds->getKind();
    if (kind == ContentDirectory::CDSKIND_MINIM) {
        // Use search() rather than a tree walk for Minim, it is much
        // more efficient.
        UPnPDirContent dirbuf;
        string ss("upnp:class = \"object.item.audioItem.musicTrack\"");
        int err = cds->search(m_popupobjid, ss, dirbuf);
        if (err != 0) {
            LOGERR("CDBrowser::recursiveAdd: search failed, code " << err);
            return;
        }

        m_recwalkentries = dirbuf.m_items;

        // If we are inside the folders view tree, and the option is
        // set, sort by url (minim sorts by tracknum tag even inside
        // folder view)
        if (CSettingsStorage::getInstance()->getFolderViewFNOrder() && 
            m_popupobjid.compare(0, minimFoldersViewPrefix.size(),
                                 minimFoldersViewPrefix) == 0) {
            sort(m_recwalkentries.begin(), m_recwalkentries.end(), 
                 dirObjCmpByURI);
        }

        rreaperDone(0);

    } else {
        if (m_reaper) {
            delete m_reaper;
            m_reaper = 0;
        }
    
        m_recwalkentries.clear();
        m_recwalkdedup.clear();
        m_reaper = new RecursiveReaper(cds, m_popupobjid, this);
        connect(m_reaper, 
                SIGNAL(sliceAvailable(const UPnPClient::UPnPDirContent *)),
                this, 
                SLOT(onReaperSliceAvailable(const UPnPClient::UPnPDirContent *)));
        connect(m_reaper, SIGNAL(done(int)), this, SLOT(rreaperDone(int)));
        m_reaper->start();
    }
}

void CDBrowser::onReaperSliceAvailable(const UPnPClient::UPnPDirContent *dc)
{
    LOGDEB1("CDBrowser::onReaperSliceAvailable: got " << 
            dc->m_items.size() << " items" << endl);
    for (unsigned int i = 0; i < dc->m_items.size(); i++) {
        if (dc->m_items[i].m_resources.empty()) {
            LOGDEB("CDBrowser::onReaperSlice: entry has no resources??"<< endl);
            continue;
        }
        //LOGDEB1("CDBrowser::onReaperSlice: uri: " << 
        //       dc->m_items[i].m_resources[0].m_uri << endl);
        string md5;
        MD5String(dc->m_items[i].m_resources[0].m_uri, md5);
        auto res = m_recwalkdedup.insert(md5);
        if (res.second) {
            m_recwalkentries.push_back(dc->m_items[i]);
        } else {
            LOGDEB("CDBrowser::onReaperSlice: dup found" << endl);
        }
    }
}

void CDBrowser::rreaperDone(int status)
{
    if (m_reaper) {
        LOGDEB("CDBrowser::rreaperDone: status: " << status << endl);
        m_reaper->wait();
        delete m_reaper;
        m_reaper = 0;
    }

    MetaDataList mdl;
    mdl.resize(m_recwalkentries.size());
    for (unsigned int i = 0; i <  m_recwalkentries.size(); i++) {
        udirentToMetadata(&m_recwalkentries[i], &mdl[i]);
    }
    emit sig_tracks_to_playlist(mdl);
}
