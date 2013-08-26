/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 2013 Zend Technologies Ltd. (http://www.zend.com)      |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/3_01.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Christopher Jones <christopher.jones@oracle.com>             |
   +----------------------------------------------------------------------+
*/

provider php {
	probe oci8__connect__start(char *username, char *dbname, char *charset, long session_mode, int persistent, int exclusive);
	probe oci8__connect__done();
	probe oci8__sqltext(char *sql);
	probe oci8__error(int status, long errcode);
	probe oci8__execute__mode(unsigned int mode);

	probe oci8__connect__p__dtor__close(void *connection);
	probe oci8__connect__p__dtor__release(void *connection);
	probe oci8__connect__lookup(void *connection, int is_stub);
	probe oci8__connect__expiry(void *connection, int is_stub, time_t idle_expiry, time_t timestamp);
	probe oci8__connect__type(int is_persistent, int exclusive, void *connection, long num_persistent, long num_links);
	probe oci8__sesspool__create(void *session_pool);
	probe oci8__sesspool__stats(unsigned long free, unsigned long busy, unsigned long open);
	probe oci8__sesspool__type(int type, void *session_pool);
};
