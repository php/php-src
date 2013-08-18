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
	probe oci8__connect_start(char *username, char *dbname, char *charset, long session_mode, int persistent, int exclusive);
	probe oci8__connect_done();
	probe oci8__sqltext(char *sql);
	probe oci8__error(int status, long errcode);
	probe oci8__execute_mode(unsigned int mode);

	probe oci8__connect_p_dtor_close(void *connection);
	probe oci8__connect_p_dtor_release(void *connection);
	probe oci8__connect_lookup(void *connection, int is_stub);
	probe oci8__connect_expiry(void *connection, int is_stub, time_t idle_expiry, time_t timestamp);
	probe oci8__connect_type(int is_persistent, int exclusive, void *connection, long num_persistent, long num_links);
	probe oci8__sesspool_create(void *session_pool);
	probe oci8__sesspool_stats(unsigned long free, unsigned long busy, unsigned long open);
	probe oci8__sesspool_type(int type, void *session_pool);

	probe oci8__ociattrget_start();
	probe oci8__ociattrget_done();
	probe oci8__ociattrset_start();
	probe oci8__ociattrset_done();
	probe oci8__ocibindbyname_start();
	probe oci8__ocibindbyname_done();
	probe oci8__ocibinddynamic_start();
	probe oci8__ocibinddynamic_done();
	probe oci8__ocibindobject_start();
	probe oci8__ocibindobject_done();
	probe oci8__ociclientversion_start();
	probe oci8__ociclientversion_done();
	probe oci8__ocicollappend_start();
	probe oci8__ocicollappend_done();
	probe oci8__ocicollassign_start();
	probe oci8__ocicollassign_done();
	probe oci8__ocicollassignelem_start();
	probe oci8__ocicollassignelem_done();
	probe oci8__ocicollgetelem_start();
	probe oci8__ocicollgetelem_done();
	probe oci8__ocicollmax_start();
	probe oci8__ocicollmax_done();
	probe oci8__ocicollsize_start();
	probe oci8__ocicollsize_done();
	probe oci8__ocicolltrim_start();
	probe oci8__ocicolltrim_done();
	probe oci8__ocicontextgetvalue_start();
	probe oci8__ocicontextgetvalue_done();
	probe oci8__ocicontextsetvalue_start();
	probe oci8__ocicontextsetvalue_done();
	probe oci8__ocidatefromtext_start();
	probe oci8__ocidatefromtext_done();
	probe oci8__ocidatetotext_start();
	probe oci8__ocidatetotext_done();
	probe oci8__ocidefinebypos_start();
	probe oci8__ocidefinebypos_done();
	probe oci8__ocidefinedynamic_start();
	probe oci8__ocidefinedynamic_done();
	probe oci8__ocidescribeany_start();
	probe oci8__ocidescribeany_done();
	probe oci8__ocidescriptoralloc_start();
	probe oci8__ocidescriptoralloc_done();
	probe oci8__ocidescriptorfree_start();
	probe oci8__ocidescriptorfree_done();
	probe oci8__ocienvnlscreate_start();
	probe oci8__ocienvnlscreate_done();
	probe oci8__ocierrorget_start();
	probe oci8__ocierrorget_done();
	probe oci8__ocihandlealloc_start();
	probe oci8__ocihandlealloc_done();
	probe oci8__ocihandlefree_start();
	probe oci8__ocihandlefree_done();
	probe oci8__ocilobappend_start();
	probe oci8__ocilobappend_done();
	probe oci8__ocilobcharsetid_start();
	probe oci8__ocilobcharsetid_done();
	probe oci8__ocilobclose_start();
	probe oci8__ocilobclose_done();
	probe oci8__ocilobcopy_start();
	probe oci8__ocilobcopy_done();
	probe oci8__ocilobcreatetemporary_start();
	probe oci8__ocilobcreatetemporary_done();
	probe oci8__ocilobdisablebuffering_start();
	probe oci8__ocilobdisablebuffering_done();
	probe oci8__ocilobenablebuffering_start();
	probe oci8__ocilobenablebuffering_done();
	probe oci8__ociloberase_start();
	probe oci8__ociloberase_done();
	probe oci8__ocilobfileclose_start();
	probe oci8__ocilobfileclose_done();
	probe oci8__ocilobfileopen_start();
	probe oci8__ocilobfileopen_done();
	probe oci8__ocilobflushbuffer_start();
	probe oci8__ocilobflushbuffer_done();
	probe oci8__ocilobfreetemporary_start();
	probe oci8__ocilobfreetemporary_done();
	probe oci8__ocilobgetchunksize_start();
	probe oci8__ocilobgetchunksize_done();
	probe oci8__ocilobgetlength_start();
	probe oci8__ocilobgetlength_done();
	probe oci8__ocilobisequal_start();
	probe oci8__ocilobisequal_done();
	probe oci8__ocilobistemporary_start();
	probe oci8__ocilobistemporary_done();
	probe oci8__ocilobopen_start();
	probe oci8__ocilobopen_done();
	probe oci8__ocilobread2_start();
	probe oci8__ocilobread2_done();
	probe oci8__ocilobtrim_start();
	probe oci8__ocilobtrim_done();
	probe oci8__ocilobwrite_start();
	probe oci8__ocilobwrite_done();
	probe oci8__ocimemoryalloc_start();
	probe oci8__ocimemoryalloc_done();
	probe oci8__ocimemoryfree_start();
	probe oci8__ocimemoryfree_done();
	probe oci8__ocinlscharsetnametoid_start();
	probe oci8__ocinlscharsetnametoid_done();
	probe oci8__ocinlsenvironmentvariableget_start();
	probe oci8__ocinlsenvironmentvariableget_done();
	probe oci8__ocinlsnumericinfoget_start();
	probe oci8__ocinlsnumericinfoget_done();
	probe oci8__ocinumberfromreal_start();
	probe oci8__ocinumberfromreal_done();
	probe oci8__ocinumbertoreal_start();
	probe oci8__ocinumbertoreal_done();
	probe oci8__ociobjectfree_start();
	probe oci8__ociobjectfree_done();
	probe oci8__ociobjectnew_start();
	probe oci8__ociobjectnew_done();
	probe oci8__ociparamget_start();
	probe oci8__ociparamget_done();
	probe oci8__ocipasswordchange_start();
	probe oci8__ocipasswordchange_done();
	probe oci8__ociping_start();
	probe oci8__ociping_done();
	probe oci8__ociserverattach_start();
	probe oci8__ociserverattach_done();
	probe oci8__ociserverdetach_start();
	probe oci8__ociserverdetach_done();
	probe oci8__ociserverversion_start();
	probe oci8__ociserverversion_done();
	probe oci8__ocisessionbegin_start();
	probe oci8__ocisessionbegin_done();
	probe oci8__ocisessionend_start();
	probe oci8__ocisessionend_done();
	probe oci8__ocisessionget_start();
	probe oci8__ocisessionget_done();
	probe oci8__ocisessionpoolcreate_start();
	probe oci8__ocisessionpoolcreate_done();
	probe oci8__ocisessionpooldestroy_start();
	probe oci8__ocisessionpooldestroy_done();
	probe oci8__ocisessionrelease_start();
	probe oci8__ocisessionrelease_done();
	probe oci8__ocistmtexecute_start();
	probe oci8__ocistmtexecute_done();
	probe oci8__ocistmtfetch_start();
	probe oci8__ocistmtfetch_done();
	probe oci8__ocistmtgetnextresult_start();
	probe oci8__ocistmtgetnextresult_done();
	probe oci8__ocistmtgetpieceinfo_start();
	probe oci8__ocistmtgetpieceinfo_done();
	probe oci8__ocistmtprepare2_start();
	probe oci8__ocistmtprepare2_done();
	probe oci8__ocistmtrelease_start();
	probe oci8__ocistmtrelease_done();
	probe oci8__ocistmtsetpieceinfo_start();
	probe oci8__ocistmtsetpieceinfo_done();
	probe oci8__ocistringassigntext_start();
	probe oci8__ocistringassigntext_done();
	probe oci8__ocistringptr_start();
	probe oci8__ocistringptr_done();
	probe oci8__ocitranscommit_start();
	probe oci8__ocitranscommit_done();
	probe oci8__ocitransrollback_start();
	probe oci8__ocitransrollback_done();
	probe oci8__ocitypebyname_start();
	probe oci8__ocitypebyname_done();
	probe oci8__ocitypebyref_start();
	probe oci8__ocitypebyref_done();
};
