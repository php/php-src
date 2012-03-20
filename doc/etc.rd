# etc.rd -  -*- mode: rd; coding: us-ascii -*- created at: Fri Jul 14 00:47:15 JST 1995
=begin

= Etc(Module)

The module to retrieve information from running OS.  All operations
defined in this module are module functions, so that you can include
Etc module into your class.

== Module Function

--- getlogin

	returns login name of the user.  It this fails, try getpwuid().

--- getpwnam(name)

	searches in /etc/passwd file (or equivalent database), and
	returns password entry for the user.  The return value is an
	passwd structure, which has members described below.

	  struct passwd
	    name 	# user name(string)
	    passwd	# encrypted password(string)
	    uid		# user ID(integer)
	    gid		# group ID(integer)
	    gecos	# gecos field(string)
	    dir		# home directory(string)
	    shell	# login shell(string)
	    # members below are optional
	    change	# password change time(integer)
	    quota	# quota value(integer)
	    age		# password age(integer)
	    class	# user access class(string)
	    comment	# comment(string)
	    expire	# account expiration time(integer)	    
	  end

	See getpwnam(3) for detail.

--- getpwuid([uid])

	returns passwd entry for the specified user id.  If uid is
	ommitted, use the value from getuid().  See getpwuid(3) for
	detail.

--- getgrgid(gid)

	searches in /etc/group file (or equivalent database), and
	returns group entry for the group id.  The return value is an
	group structure, which has members described below.

	  struct group
	    name 	# group name(string)
	    passwd	# group password(string)
	    gid		# group ID(integer)
	    mem		# array of the group member names
	  end

	See getgrgid(3) for detail.

--- getgrnam(name)

	returns the group entry for the specified name.  The return
	value is the group structure.  See getgrnam(3) for detail.

--- group

	iterates over all group entries.

--- passwd

	iterates over all passwd entries.

=end
