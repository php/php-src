################################################################
# File:         /etc/init.d/phpdbg                             #
# Author:       krakjoe                                        #
# Purpose:      Daemonize phpdbg automatically on boot         #
# chkconfig:    2345    07 09                                  #
# description:  Starts, stops and restarts phpdbg daemon       #
################################################################
LOCKFILE=/var/lock/subsys/phpdbg
PIDFILE=/var/run/phpdbg.pid
STDIN=4000
STDOUT=8000
################################################################
# Either set path to phpdbg here or rely on phpdbg in ENV/PATH #
################################################################
if [ "x${PHPDBG}" == "x" ]; then
	PHPDBG=$(which phpdbg 2>/dev/null)
fi
################################################################
# Options to pass to phpdbg upon boot                          #
################################################################
OPTIONS=
LOGFILE=/var/log/phpdbg.log
################################################################
#     STOP EDITING STOP EDITING STOP EDITING STOP EDITING      #
################################################################
. /etc/rc.d/init.d/functions
RETVAL=1
################################################################
insanity()
{
	if [ "x${PHPDBG}" == "x" ]; then
		PHPDBG=$(which phpdbg 2>>/dev/null)
		if [ $? != 0 ]; then
			echo -n $"Fatal: cannot find phpdbg ${PHPDBG}"
			echo_failure
			echo
			return 1
		fi
	else
		if [ ! -x ${PHPDBG} ]; then
			echo -n $"Fatal: cannot execute phpdbg ${PHPDBG}"
			echo_failure
			echo
			return 1
		fi
	fi

	return 0
}

start()
{
	insanity

	if [ $? -eq 1 ]; then
		return $RETVAL
	fi

	echo -n $"Starting: phpdbg ${OPTIONS} on ${STDIN}/${STDOUT} "
	nohup ${PHPDBG} -l${STDIN}/${STDOUT} ${OPTIONS} 2>>${LOGFILE} 1>/dev/null </dev/null &
	PID=$!
	RETVAL=$?
	if [ $RETVAL -eq 0 ]; then
		echo $PID > $PIDFILE
		echo_success
	else
		echo_failure
	fi
	echo
	[ $RETVAL = 0 ] && touch ${LOCKFILE}
	return $RETVAL
}

stop()
{
	insanity

	if [ $? -eq 1 ]; then
		return $RETVAL
	fi

	if [ -f ${LOCKFILE} ] && [ -f ${PIDFILE} ]
	then
		echo -n $"Stopping: phpdbg ${OPTIONS} on ${STDIN}/${STDOUT} "
		kill -s TERM $(cat $PIDFILE)
		RETVAL=$?
		if [ $RETVAL -eq 0 ]; then
			echo_success
		else
			echo_failure
		fi
		echo
		[ $RETVAL = 0 ] && rm -f ${LOCKFILE} ${PIDFILE}
	else
		echo -n $"Error: phpdbg not running"
		echo_failure
		echo
		[ $RETVAL = 1 ]
	fi
	return $RETVAL
}
##################################################################
case "$1" in
	start)
	start
	;;
	stop)
	stop
	;;
	status)
	status $PHPDBG
	;;
	restart)
	$0 stop
	$0 start
	;;
	*)
	echo "usage: $0 start|stop|restart|status"
	;;
esac
###################################################################
exit $RETVAL
