#! /bin/sh
### BEGIN INIT INFO
# Provides:          RaspiDAC
# Required-Start:    $remote_fs $syslog mpd
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Initscript for RaspiDAC
# Description:       This file should be used to construct scripts to be
#                    placed in /etc/init.d.
### END INIT INFO

# Author: Florian Fliegenpilz <flo.fp@gmx.de>
#

# Do NOT "set -e"

# PATH should only include /usr/* if it runs after the mountnfs.sh script
PATH=/bin:/sbin:/usr/sbin:/usr/bin:/home/pi/RaspiDAC
DESC="RaspiDAC Steuerung"
NAME=RaspiDAC
DAEMON=/home/pi/RaspiDAC/start.sh
PIDFILE=/var/run/RaspiDAC.pid
SCRIPTNAME=/etc/init.d/RaspiDAC
LOG_PATH=/var/log/RaspiDAC                   # Standard output and Standard error will be outputted here

# Exit if the package is not installed
[ -x "$DAEMON" ] || exit 0

# Read configuration variable file if it is present
[ -r /etc/default/$NAME ] && . /etc/default/$NAME

# Load the VERBOSE setting and other rcS variables
#. /lib/init/vars.sh

# Define LSB log_* functions.
# Depend on lsb-base (>= 3.2-14) to ensure that this file is present
# and status_of_proc is working.
. /lib/lsb/init-functions

#
# Function that starts the daemon/service
#
do_start()
{
  # Return
  #  0 if daemon has been started
  #  1 if daemon was already running
  #  2 if daemon could not be started
  #echo $NAME
  #pid=$(pidof -x $NAME)
  #echo $pid
  #test -n "$pid" && return 1
  #echo test1
  start-stop-daemon --background --quiet --start --pidfile $PIDFILE --exec $DAEMON --test &> ${LOG_PATH}/${NAME}.log || return 1
  #echo test2
  start-stop-daemon --background --start --quiet --make-pidfile --pidfile $PIDFILE --exec $DAEMON  &> ${LOG_PATH}/${NAME}.log || return 2
  #echo $DAEMON
  #sleep 0.3
  #pid=$(pidof -x $NAME)
  #test -z "$pid" && { rm $pidfile; return 2; }
}

#
# Function that stops the daemon/service
#
do_stop()
{
  # Return
  #  0 if daemon has been stopped
  #  1 if daemon was already stopped
  #  2 if daemon could not be stopped
  start-stop-daemon --stop --quiet --verbose --retry=TERM/30/KILL/5 --pidfile $PIDFILE
  RETVAL="$?"
  [ "$RETVAL" = 2 ] && return 2
  start-stop-daemon --stop --quiet --oknodo --verbose --retry=TERM/30/KILL/5 --name $NAME
  RETVAL="$?"
  [ "$RETVAL" = 2 ] && return 2
  # Many daemons don't delete their pidfiles when they exit.
  rm -f $PIDFILE
  return 0
  #return "$RETVAL"
}

#
# Function that sends a SIGHUP to the daemon/service
#
#do_reload() {
	#
	# If the daemon can reload its configuration without
	# restarting (for example, when it is sent a SIGHUP),
	# then implement that here.
	#
#	start-stop-daemon --stop --quiet --signal 1 --pidfile $PIDFILE --exec $NAME
#	return 0
#}

case "$1" in
  start)
	[ "$VERBOSE" != no ] && log_daemon_msg "Starting $DESC" "$NAME"
	do_start
	case "$?" in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0 ;;
		2) [ "$VERBOSE" != no ] && log_end_msg 1 ;;
	esac
	;;
  stop)
	[ "$VERBOSE" != no ] && log_daemon_msg "Stopping $DESC" "$NAME"
	do_stop
	case "$?" in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0 ;;
		2) [ "$VERBOSE" != no ] && log_end_msg 1 ;;
	esac
	;;
  status)
	status_of_proc "$DAEMON" "$NAME" && exit 0 || exit $?
	;;
  #reload|force-reload)
	#
	# If do_reload() is not implemented then leave this commented out
	# and leave 'force-reload' as an alias for 'restart'.
	#
	#log_daemon_msg "Reloading $DESC" "$NAME"
	#do_reload
	#log_end_msg $?
	#;;
  restart|force-reload)
	#
	# If the "reload" option is implemented then remove the
	# 'force-reload' alias
	#
	log_daemon_msg "Restarting $DESC" "$NAME"
	do_stop
	case "$?" in
	  0|1)
		do_start
		case "$?" in
			0) log_end_msg 0 ;;
			1) log_end_msg 1 ;; # Old process is still running
			*) log_end_msg 1 ;; # Failed to start
		esac
		;;
	  *)
		# Failed to stop
		log_end_msg 1
		;;
	esac
	;;
  *)
	#echo "Usage: $SCRIPTNAME {start|stop|restart|reload|force-reload}" >&2
	echo "Usage: $SCRIPTNAME {start|stop|status|restart|force-reload}" >&2
	exit 3
	;;
esac

:

