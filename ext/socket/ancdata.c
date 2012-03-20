#include "rubysocket.h"

#include <time.h>

#if defined(HAVE_ST_MSG_CONTROL)
static VALUE rb_cAncillaryData;

static VALUE
constant_to_sym(int constant, ID (*intern_const)(int))
{
    ID name = intern_const(constant);
    if (name) {
        return ID2SYM(name);
    }

    return INT2NUM(constant);
}

static VALUE
ip_cmsg_type_to_sym(int level, int cmsg_type)
{
    switch (level) {
      case SOL_SOCKET:
        return constant_to_sym(cmsg_type, rsock_intern_scm_optname);
      case IPPROTO_IP:
        return constant_to_sym(cmsg_type, rsock_intern_ip_optname);
#ifdef IPPROTO_IPV6
      case IPPROTO_IPV6:
        return constant_to_sym(cmsg_type, rsock_intern_ipv6_optname);
#endif
      case IPPROTO_TCP:
        return constant_to_sym(cmsg_type, rsock_intern_tcp_optname);
      case IPPROTO_UDP:
        return constant_to_sym(cmsg_type, rsock_intern_udp_optname);
      default:
        return INT2NUM(cmsg_type);
    }
}

/*
 * call-seq:
 *   Socket::AncillaryData.new(family, cmsg_level, cmsg_type, cmsg_data) -> ancillarydata
 *
 * _family_ should be an integer, a string or a symbol.
 * - Socket::AF_INET, "AF_INET", "INET", :AF_INET, :INET
 * - Socket::AF_UNIX, "AF_UNIX", "UNIX", :AF_UNIX, :UNIX
 * - etc.
 *
 * _cmsg_level_ should be an integer, a string or a symbol.
 * - Socket::SOL_SOCKET, "SOL_SOCKET", "SOCKET", :SOL_SOCKET and :SOCKET
 * - Socket::IPPROTO_IP, "IP" and :IP
 * - Socket::IPPROTO_IPV6, "IPV6" and :IPV6
 * - Socket::IPPROTO_TCP, "TCP" and :TCP
 * - etc.
 *
 * _cmsg_type_ should be an integer, a string or a symbol.
 * If a string/symbol is specified, it is interpreted depend on _cmsg_level_.
 * - Socket::SCM_RIGHTS, "SCM_RIGHTS", "RIGHTS", :SCM_RIGHTS, :RIGHTS for SOL_SOCKET
 * - Socket::IP_RECVTTL, "RECVTTL" and :RECVTTL for IPPROTO_IP
 * - Socket::IPV6_PKTINFO, "PKTINFO" and :PKTINFO for IPPROTO_IPV6
 * - etc.
 *
 * _cmsg_data_ should be a string.
 *
 *   p Socket::AncillaryData.new(:INET, :TCP, :NODELAY, "")
 *   #=> #<Socket::AncillaryData: INET TCP NODELAY "">
 *
 *   p Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "")
 *   #=> #<Socket::AncillaryData: INET6 IPV6 PKTINFO "">
 *
 */
static VALUE
ancillary_initialize(VALUE self, VALUE vfamily, VALUE vlevel, VALUE vtype, VALUE data)
{
    int family = rsock_family_arg(vfamily);
    int level = rsock_level_arg(family, vlevel);
    int type = rsock_cmsg_type_arg(family, level, vtype);
    StringValue(data);
    rb_ivar_set(self, rb_intern("family"), INT2NUM(family));
    rb_ivar_set(self, rb_intern("level"), INT2NUM(level));
    rb_ivar_set(self, rb_intern("type"), INT2NUM(type));
    rb_ivar_set(self, rb_intern("data"), data);
    return self;
}

static VALUE
ancdata_new(int family, int level, int type, VALUE data)
{
    NEWOBJ(obj, struct RObject);
    OBJSETUP(obj, rb_cAncillaryData, T_OBJECT);
    StringValue(data);
    ancillary_initialize((VALUE)obj, INT2NUM(family), INT2NUM(level), INT2NUM(type), data);
    return (VALUE)obj;
}

static int
ancillary_family(VALUE self)
{
    VALUE v = rb_attr_get(self, rb_intern("family"));
    return NUM2INT(v);
}

/*
 * call-seq:
 *   ancillarydata.family => integer
 *
 * returns the socket family as an integer.
 *
 *   p Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "").family
 *   #=> 10
 */
static VALUE
ancillary_family_m(VALUE self)
{
    return INT2NUM(ancillary_family(self));
}

static int
ancillary_level(VALUE self)
{
    VALUE v = rb_attr_get(self, rb_intern("level"));
    return NUM2INT(v);
}

/*
 * call-seq:
 *   ancillarydata.level => integer
 *
 * returns the cmsg level as an integer.
 *
 *   p Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "").level
 *   #=> 41
 */
static VALUE
ancillary_level_m(VALUE self)
{
    return INT2NUM(ancillary_level(self));
}

static int
ancillary_type(VALUE self)
{
    VALUE v = rb_attr_get(self, rb_intern("type"));
    return NUM2INT(v);
}

/*
 * call-seq:
 *   ancillarydata.type => integer
 *
 * returns the cmsg type as an integer.
 *
 *   p Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "").type
 *   #=> 2
 */
static VALUE
ancillary_type_m(VALUE self)
{
    return INT2NUM(ancillary_type(self));
}

/*
 * call-seq:
 *   ancillarydata.data => string
 *
 * returns the cmsg data as a string.
 *
 *   p Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "").data
 *   #=> ""
 */
static VALUE
ancillary_data(VALUE self)
{
    VALUE v = rb_attr_get(self, rb_intern("data"));
    StringValue(v);
    return v;
}

#ifdef SCM_RIGHTS
/*
 * call-seq:
 *   Socket::AncillaryData.unix_rights(io1, io2, ...) => ancillarydata
 *
 * Creates a new Socket::AncillaryData object which contains file descriptors as data.
 *
 *   p Socket::AncillaryData.unix_rights(STDERR)
 *   #=> #<Socket::AncillaryData: UNIX SOCKET RIGHTS 2>
 */
static VALUE
ancillary_s_unix_rights(int argc, VALUE *argv, VALUE klass)
{
    VALUE result, str, ary;
    int i;

    ary = rb_ary_new();

    for (i = 0 ; i < argc; i++) {
        VALUE obj = argv[i];
        if (TYPE(obj) != T_FILE) {
            rb_raise(rb_eTypeError, "IO expected");
        }
        rb_ary_push(ary, obj);
    }

    str = rb_str_buf_new(sizeof(int) * argc);

    for (i = 0 ; i < argc; i++) {
        VALUE obj = RARRAY_PTR(ary)[i];
        rb_io_t *fptr;
        int fd;
        GetOpenFile(obj, fptr);
        fd = fptr->fd;
        rb_str_buf_cat(str, (char *)&fd, sizeof(int));
    }

    result = ancdata_new(AF_UNIX, SOL_SOCKET, SCM_RIGHTS, str);
    rb_ivar_set(result, rb_intern("unix_rights"), ary);
    return result;
}
#else
#define ancillary_s_unix_rights rb_f_notimplement
#endif

#ifdef SCM_RIGHTS
/*
 * call-seq:
 *   ancillarydata.unix_rights => array-of-IOs or nil
 *
 * returns the array of IO objects for SCM_RIGHTS control message in UNIX domain socket.
 *
 * The class of the IO objects in the array is IO or Socket.
 *
 * The array is attached to _ancillarydata_ when it is instantiated.
 * For example, BasicSocket#recvmsg attach the array when
 * receives a SCM_RIGHTS control message and :scm_rights=>true option is given.
 *
 *   # recvmsg needs :scm_rights=>true for unix_rights
 *   s1, s2 = UNIXSocket.pair
 *   p s1                                         #=> #<UNIXSocket:fd 3>
 *   s1.sendmsg "stdin and a socket", 0, nil, Socket::AncillaryData.unix_rights(STDIN, s1)
 *   _, _, _, ctl = s2.recvmsg(:scm_rights=>true)
 *   p ctl                                        #=> #<Socket::AncillaryData: UNIX SOCKET RIGHTS 6 7>
 *   p ctl.unix_rights                            #=> [#<IO:fd 6>, #<Socket:fd 7>]
 *   p File.identical?(STDIN, ctl.unix_rights[0]) #=> true
 *   p File.identical?(s1, ctl.unix_rights[1])    #=> true
 *
 *   # If :scm_rights=>true is not given, unix_rights returns nil
 *   s1, s2 = UNIXSocket.pair
 *   s1.sendmsg "stdin and a socket", 0, nil, Socket::AncillaryData.unix_rights(STDIN, s1)
 *   _, _, _, ctl = s2.recvmsg
 *   p ctl #=> #<Socket::AncillaryData: UNIX SOCKET RIGHTS 6 7>
 *   p ctl.unix_rights #=> nil
 *
 */
static VALUE
ancillary_unix_rights(VALUE self)
{
    int level, type;

    level = ancillary_level(self);
    type = ancillary_type(self);

    if (level != SOL_SOCKET || type != SCM_RIGHTS)
        rb_raise(rb_eTypeError, "SCM_RIGHTS ancillary data expected");

    return rb_attr_get(self, rb_intern("unix_rights"));
}
#else
#define ancillary_unix_rights rb_f_notimplement
#endif

#if defined(SCM_TIMESTAMP) || defined(SCM_TIMESTAMPNS) || defined(SCM_BINTIME)
/*
 * call-seq:
 *   ancillarydata.timestamp => time
 *
 * returns the timestamp as a time object.
 *
 * _ancillarydata_ should be one of following type:
 * - SOL_SOCKET/SCM_TIMESTAMP (micro second) GNU/Linux, FreeBSD, NetBSD, OpenBSD, Solaris, MacOS X
 * - SOL_SOCKET/SCM_TIMESTAMPNS (nano second) GNU/Linux
 * - SOL_SOCKET/SCM_BINTIME (2**(-64) second) FreeBSD
 *
 *   Addrinfo.udp("127.0.0.1", 0).bind {|s1|
 *     Addrinfo.udp("127.0.0.1", 0).bind {|s2|
 *       s1.setsockopt(:SOCKET, :TIMESTAMP, true)
 *       s2.send "a", 0, s1.local_address
 *       ctl = s1.recvmsg.last
 *       p ctl    #=> #<Socket::AncillaryData: INET SOCKET TIMESTAMP 2009-02-24 17:35:46.775581>
 *       t = ctl.timestamp
 *       p t      #=> 2009-02-24 17:35:46 +0900
 *       p t.usec #=> 775581
 *       p t.nsec #=> 775581000
 *     }
 *   }
 *
 */
static VALUE
ancillary_timestamp(VALUE self)
{
    int level, type;
    VALUE data;
    VALUE result = Qnil;

    level = ancillary_level(self);
    type = ancillary_type(self);
    data = ancillary_data(self);

# ifdef SCM_TIMESTAMP
    if (level == SOL_SOCKET && type == SCM_TIMESTAMP &&
        RSTRING_LEN(data) == sizeof(struct timeval)) {
        struct timeval tv;
        memcpy((char*)&tv, RSTRING_PTR(data), sizeof(tv));
        result = rb_time_new(tv.tv_sec, tv.tv_usec);
    }
# endif

# ifdef SCM_TIMESTAMPNS
    if (level == SOL_SOCKET && type == SCM_TIMESTAMPNS &&
        RSTRING_LEN(data) == sizeof(struct timespec)) {
        struct timespec ts;
        memcpy((char*)&ts, RSTRING_PTR(data), sizeof(ts));
        result = rb_time_nano_new(ts.tv_sec, ts.tv_nsec);
    }
# endif

#define add(x,y) (rb_funcall((x), '+', 1, (y)))
#define mul(x,y) (rb_funcall((x), '*', 1, (y)))
#define quo(x,y) (rb_funcall((x), rb_intern("quo"), 1, (y)))

# ifdef SCM_BINTIME
    if (level == SOL_SOCKET && type == SCM_BINTIME &&
        RSTRING_LEN(data) == sizeof(struct bintime)) {
        struct bintime bt;
	VALUE d, timev;
        memcpy((char*)&bt, RSTRING_PTR(data), sizeof(bt));
	d = ULL2NUM(0x100000000ULL);
	d = mul(d,d);
	timev = add(TIMET2NUM(bt.sec), quo(ULL2NUM(bt.frac), d));
        result = rb_time_num_new(timev, Qnil);
    }
# endif

    if (result == Qnil)
        rb_raise(rb_eTypeError, "timestamp ancillary data expected");

    return result;
}
#else
#define ancillary_timestamp rb_f_notimplement
#endif

/*
 * call-seq:
 *   Socket::AncillaryData.int(family, cmsg_level, cmsg_type, integer) => ancillarydata
 *
 * Creates a new Socket::AncillaryData object which contains a int as data.
 *
 * The size and endian is dependent on the host.
 *
 *   p Socket::AncillaryData.int(:UNIX, :SOCKET, :RIGHTS, STDERR.fileno)
 *   #=> #<Socket::AncillaryData: UNIX SOCKET RIGHTS 2>
 */
static VALUE
ancillary_s_int(VALUE klass, VALUE vfamily, VALUE vlevel, VALUE vtype, VALUE integer)
{
    int family = rsock_family_arg(vfamily);
    int level = rsock_level_arg(family, vlevel);
    int type = rsock_cmsg_type_arg(family, level, vtype);
    int i = NUM2INT(integer);
    return ancdata_new(family, level, type, rb_str_new((char*)&i, sizeof(i)));
}

/*
 * call-seq:
 *   ancillarydata.int => integer
 *
 * Returns the data in _ancillarydata_ as an int.
 *
 * The size and endian is dependent on the host.
 *
 *   ancdata = Socket::AncillaryData.int(:UNIX, :SOCKET, :RIGHTS, STDERR.fileno)
 *   p ancdata.int #=> 2
 */
static VALUE
ancillary_int(VALUE self)
{
    VALUE data;
    int i;
    data = ancillary_data(self);
    if (RSTRING_LEN(data) != sizeof(int))
        rb_raise(rb_eTypeError, "size differ.  expected as sizeof(int)=%d but %ld", (int)sizeof(int), (long)RSTRING_LEN(data));
    memcpy((char*)&i, RSTRING_PTR(data), sizeof(int));
    return INT2NUM(i);
}

#if defined(IPPROTO_IP) && defined(IP_PKTINFO) && defined(HAVE_STRUCT_IN_PKTINFO_IPI_SPEC_DST) /* GNU/Linux */
/*
 * call-seq:
 *   Socket::AncillaryData.ip_pktinfo(addr, ifindex) => ancdata
 *   Socket::AncillaryData.ip_pktinfo(addr, ifindex, spec_dst) => ancdata
 *
 * Returns new ancillary data for IP_PKTINFO.
 *
 * If spec_dst is not given, addr is used.
 *
 * IP_PKTINFO is not standard.
 *
 * Supported platform: GNU/Linux
 *
 *   addr = Addrinfo.ip("127.0.0.1")
 *   ifindex = 0
 *   spec_dst = Addrinfo.ip("127.0.0.1")
 *   p Socket::AncillaryData.ip_pktinfo(addr, ifindex, spec_dst)
 *   #=> #<Socket::AncillaryData: INET IP PKTINFO 127.0.0.1 ifindex:0 spec_dst:127.0.0.1>
 *
 */
static VALUE
ancillary_s_ip_pktinfo(int argc, VALUE *argv, VALUE self)
{
    VALUE v_addr, v_ifindex, v_spec_dst;
    unsigned int ifindex;
    struct sockaddr_in sa;
    struct in_pktinfo pktinfo;

    rb_scan_args(argc, argv, "21", &v_addr, &v_ifindex, &v_spec_dst);

    SockAddrStringValue(v_addr);
    ifindex = NUM2UINT(v_ifindex);
    if (NIL_P(v_spec_dst))
        v_spec_dst = v_addr;
    else
        SockAddrStringValue(v_spec_dst);

    memset(&pktinfo, 0, sizeof(pktinfo));

    memset(&sa, 0, sizeof(sa));
    if (RSTRING_LEN(v_addr) != sizeof(sa))
        rb_raise(rb_eArgError, "addr size different to AF_INET sockaddr");
    memcpy(&sa, RSTRING_PTR(v_addr), sizeof(sa));
    if (sa.sin_family != AF_INET)
        rb_raise(rb_eArgError, "addr is not AF_INET sockaddr");
    memcpy(&pktinfo.ipi_addr, &sa.sin_addr, sizeof(pktinfo.ipi_addr));

    pktinfo.ipi_ifindex = ifindex;

    memset(&sa, 0, sizeof(sa));
    if (RSTRING_LEN(v_spec_dst) != sizeof(sa))
        rb_raise(rb_eArgError, "spec_dat size different to AF_INET sockaddr");
    memcpy(&sa, RSTRING_PTR(v_spec_dst), sizeof(sa));
    if (sa.sin_family != AF_INET)
        rb_raise(rb_eArgError, "spec_dst is not AF_INET sockaddr");
    memcpy(&pktinfo.ipi_spec_dst, &sa.sin_addr, sizeof(pktinfo.ipi_spec_dst));

    return ancdata_new(AF_INET, IPPROTO_IP, IP_PKTINFO, rb_str_new((char *)&pktinfo, sizeof(pktinfo)));
}
#else
#define ancillary_s_ip_pktinfo rb_f_notimplement
#endif

#if defined(IPPROTO_IP) && defined(IP_PKTINFO) && defined(HAVE_STRUCT_IN_PKTINFO_IPI_SPEC_DST) /* GNU/Linux */
/*
 * call-seq:
 *   ancdata.ip_pktinfo => [addr, ifindex, spec_dst]
 *
 * Extracts addr, ifindex and spec_dst from IP_PKTINFO ancillary data.
 *
 * IP_PKTINFO is not standard.
 *
 * Supported platform: GNU/Linux
 *
 *   addr = Addrinfo.ip("127.0.0.1")
 *   ifindex = 0
 *   spec_dest = Addrinfo.ip("127.0.0.1")
 *   ancdata = Socket::AncillaryData.ip_pktinfo(addr, ifindex, spec_dest)
 *   p ancdata.ip_pktinfo
 *   #=> [#<Addrinfo: 127.0.0.1>, 0, #<Addrinfo: 127.0.0.1>]
 *
 *
 */
static VALUE
ancillary_ip_pktinfo(VALUE self)
{
    int level, type;
    VALUE data;
    struct in_pktinfo pktinfo;
    struct sockaddr_in sa;
    VALUE v_spec_dst, v_addr;

    level = ancillary_level(self);
    type = ancillary_type(self);
    data = ancillary_data(self);

    if (level != IPPROTO_IP || type != IP_PKTINFO ||
        RSTRING_LEN(data) != sizeof(struct in_pktinfo)) {
        rb_raise(rb_eTypeError, "IP_PKTINFO ancillary data expected");
    }

    memcpy(&pktinfo, RSTRING_PTR(data), sizeof(struct in_pktinfo));
    memset(&sa, 0, sizeof(sa));

    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr, &pktinfo.ipi_addr, sizeof(sa.sin_addr));
    v_addr = rsock_addrinfo_new((struct sockaddr *)&sa, sizeof(sa), PF_INET, 0, 0, Qnil, Qnil);

    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr, &pktinfo.ipi_spec_dst, sizeof(sa.sin_addr));
    v_spec_dst = rsock_addrinfo_new((struct sockaddr *)&sa, sizeof(sa), PF_INET, 0, 0, Qnil, Qnil);

    return rb_ary_new3(3, v_addr, UINT2NUM(pktinfo.ipi_ifindex), v_spec_dst);
}
#else
#define ancillary_ip_pktinfo rb_f_notimplement
#endif

#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO) /* IPv6 RFC3542 */
/*
 * call-seq:
 *   Socket::AncillaryData.ipv6_pktinfo(addr, ifindex) => ancdata
 *
 * Returns new ancillary data for IPV6_PKTINFO.
 *
 * IPV6_PKTINFO is defined by RFC 3542.
 *
 *   addr = Addrinfo.ip("::1")
 *   ifindex = 0
 *   p Socket::AncillaryData.ipv6_pktinfo(addr, ifindex)
 *   #=> #<Socket::AncillaryData: INET6 IPV6 PKTINFO ::1 ifindex:0>
 *
 */
static VALUE
ancillary_s_ipv6_pktinfo(VALUE self, VALUE v_addr, VALUE v_ifindex)
{
    unsigned int ifindex;
    struct sockaddr_in6 sa;
    struct in6_pktinfo pktinfo;

    SockAddrStringValue(v_addr);
    ifindex = NUM2UINT(v_ifindex);

    memset(&pktinfo, 0, sizeof(pktinfo));

    memset(&sa, 0, sizeof(sa));
    if (RSTRING_LEN(v_addr) != sizeof(sa))
        rb_raise(rb_eArgError, "addr size different to AF_INET6 sockaddr");
    memcpy(&sa, RSTRING_PTR(v_addr), sizeof(sa));
    if (sa.sin6_family != AF_INET6)
        rb_raise(rb_eArgError, "addr is not AF_INET6 sockaddr");
    memcpy(&pktinfo.ipi6_addr, &sa.sin6_addr, sizeof(pktinfo.ipi6_addr));

    pktinfo.ipi6_ifindex = ifindex;

    return ancdata_new(AF_INET6, IPPROTO_IPV6, IPV6_PKTINFO, rb_str_new((char *)&pktinfo, sizeof(pktinfo)));
}
#else
#define ancillary_s_ipv6_pktinfo rb_f_notimplement
#endif

#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO) /* IPv6 RFC3542 */
static void
extract_ipv6_pktinfo(VALUE self, struct in6_pktinfo *pktinfo_ptr, struct sockaddr_in6 *sa_ptr)
{
    int level, type;
    VALUE data;

    level = ancillary_level(self);
    type = ancillary_type(self);
    data = ancillary_data(self);

    if (level != IPPROTO_IPV6 || type != IPV6_PKTINFO ||
        RSTRING_LEN(data) != sizeof(struct in6_pktinfo)) {
        rb_raise(rb_eTypeError, "IPV6_PKTINFO ancillary data expected");
    }

    memcpy(pktinfo_ptr, RSTRING_PTR(data), sizeof(*pktinfo_ptr));

    memset(sa_ptr, 0, sizeof(*sa_ptr));
    SET_SA_LEN((struct sockaddr *)sa_ptr, sizeof(struct sockaddr_in6));
    sa_ptr->sin6_family = AF_INET6;
    memcpy(&sa_ptr->sin6_addr, &pktinfo_ptr->ipi6_addr, sizeof(sa_ptr->sin6_addr));
    if (IN6_IS_ADDR_LINKLOCAL(&sa_ptr->sin6_addr))
        sa_ptr->sin6_scope_id = pktinfo_ptr->ipi6_ifindex;
}
#endif

#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO) /* IPv6 RFC3542 */
/*
 * call-seq:
 *   ancdata.ipv6_pktinfo => [addr, ifindex]
 *
 * Extracts addr and ifindex from IPV6_PKTINFO ancillary data.
 *
 * IPV6_PKTINFO is defined by RFC 3542.
 *
 *   addr = Addrinfo.ip("::1")
 *   ifindex = 0
 *   ancdata = Socket::AncillaryData.ipv6_pktinfo(addr, ifindex)
 *   p ancdata.ipv6_pktinfo #=> [#<Addrinfo: ::1>, 0]
 *
 */
static VALUE
ancillary_ipv6_pktinfo(VALUE self)
{
    struct in6_pktinfo pktinfo;
    struct sockaddr_in6 sa;
    VALUE v_addr;

    extract_ipv6_pktinfo(self, &pktinfo, &sa);
    v_addr = rsock_addrinfo_new((struct sockaddr *)&sa, (socklen_t)sizeof(sa), PF_INET6, 0, 0, Qnil, Qnil);
    return rb_ary_new3(2, v_addr, UINT2NUM(pktinfo.ipi6_ifindex));
}
#else
#define ancillary_ipv6_pktinfo rb_f_notimplement
#endif

#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO) /* IPv6 RFC3542 */
/*
 * call-seq:
 *   ancdata.ipv6_pktinfo_addr => addr
 *
 * Extracts addr from IPV6_PKTINFO ancillary data.
 *
 * IPV6_PKTINFO is defined by RFC 3542.
 *
 *   addr = Addrinfo.ip("::1")
 *   ifindex = 0
 *   ancdata = Socket::AncillaryData.ipv6_pktinfo(addr, ifindex)
 *   p ancdata.ipv6_pktinfo_addr #=> #<Addrinfo: ::1>
 *
 */
static VALUE
ancillary_ipv6_pktinfo_addr(VALUE self)
{
    struct in6_pktinfo pktinfo;
    struct sockaddr_in6 sa;
    extract_ipv6_pktinfo(self, &pktinfo, &sa);
    return rsock_addrinfo_new((struct sockaddr *)&sa, (socklen_t)sizeof(sa), PF_INET6, 0, 0, Qnil, Qnil);
}
#else
#define ancillary_ipv6_pktinfo_addr rb_f_notimplement
#endif

#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO) /* IPv6 RFC3542 */
/*
 * call-seq:
 *   ancdata.ipv6_pktinfo_ifindex => addr
 *
 * Extracts ifindex from IPV6_PKTINFO ancillary data.
 *
 * IPV6_PKTINFO is defined by RFC 3542.
 *
 *   addr = Addrinfo.ip("::1")
 *   ifindex = 0
 *   ancdata = Socket::AncillaryData.ipv6_pktinfo(addr, ifindex)
 *   p ancdata.ipv6_pktinfo_ifindex #=> 0
 *
 */
static VALUE
ancillary_ipv6_pktinfo_ifindex(VALUE self)
{
    struct in6_pktinfo pktinfo;
    struct sockaddr_in6 sa;
    extract_ipv6_pktinfo(self, &pktinfo, &sa);
    return UINT2NUM(pktinfo.ipi6_ifindex);
}
#else
#define ancillary_ipv6_pktinfo_ifindex rb_f_notimplement
#endif

#if defined(SOL_SOCKET) && defined(SCM_RIGHTS) /* 4.4BSD */
static int
anc_inspect_socket_rights(int level, int type, VALUE data, VALUE ret)
{
    if (level == SOL_SOCKET && type == SCM_RIGHTS &&
        0 < RSTRING_LEN(data) && (RSTRING_LEN(data) % sizeof(int) == 0)) {
        long off;
        for (off = 0; off < RSTRING_LEN(data); off += sizeof(int)) {
            int fd;
            memcpy((char*)&fd, RSTRING_PTR(data)+off, sizeof(int));
            rb_str_catf(ret, " %d", fd);
        }
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(SCM_CREDENTIALS) /* GNU/Linux */
static int
anc_inspect_passcred_credentials(int level, int type, VALUE data, VALUE ret)
{
    if (level == SOL_SOCKET && type == SCM_CREDENTIALS &&
        RSTRING_LEN(data) == sizeof(struct ucred)) {
        struct ucred cred;
        memcpy(&cred, RSTRING_PTR(data), sizeof(struct ucred));
        rb_str_catf(ret, " pid=%u uid=%u gid=%u", cred.pid, cred.uid, cred.gid);
	rb_str_cat2(ret, " (ucred)");
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(SCM_CREDS)
#define INSPECT_SCM_CREDS
static int
anc_inspect_socket_creds(int level, int type, VALUE data, VALUE ret)
{
    if (level != SOL_SOCKET && type != SCM_CREDS)
	return 0;

    /*
     * FreeBSD has struct cmsgcred and struct sockcred.
     * They use both SOL_SOCKET/SCM_CREDS in the ancillary message.
     * They are not ambiguous from the view of the caller
     * because struct sockcred is sent if and only if the caller sets LOCAL_CREDS socket option.
     * But inspect method doesn't know it.
     * So they are ambiguous from the view of inspect.
     * This function distinguish them by the size of the ancillary message.
     * This heuristics works well except when sc_ngroups == CMGROUP_MAX.
     */

#if defined(HAVE_TYPE_STRUCT_CMSGCRED) /* FreeBSD */
    if (RSTRING_LEN(data) == sizeof(struct cmsgcred)) {
	struct cmsgcred cred;
        memcpy(&cred, RSTRING_PTR(data), sizeof(struct cmsgcred));
        rb_str_catf(ret, " pid=%u", cred.cmcred_pid);
        rb_str_catf(ret, " uid=%u", cred.cmcred_uid);
        rb_str_catf(ret, " euid=%u", cred.cmcred_euid);
        rb_str_catf(ret, " gid=%u", cred.cmcred_gid);
	if (cred.cmcred_ngroups) {
	    int i;
	    const char *sep = " groups=";
	    for (i = 0; i < cred.cmcred_ngroups; i++) {
		rb_str_catf(ret, "%s%u", sep, cred.cmcred_groups[i]);
		sep = ",";
	    }
	}
	rb_str_cat2(ret, " (cmsgcred)");
        return 1;
    }
#endif
#if defined(HAVE_TYPE_STRUCT_SOCKCRED) /* FreeBSD, NetBSD */
    if ((size_t)RSTRING_LEN(data) >= SOCKCREDSIZE(0)) {
	struct sockcred cred0, *cred;
        memcpy(&cred0, RSTRING_PTR(data), SOCKCREDSIZE(0));
	if ((size_t)RSTRING_LEN(data) == SOCKCREDSIZE(cred0.sc_ngroups)) {
	    cred = (struct sockcred *)ALLOCA_N(char, SOCKCREDSIZE(cred0.sc_ngroups));
	    memcpy(cred, RSTRING_PTR(data), SOCKCREDSIZE(cred0.sc_ngroups));
	    rb_str_catf(ret, " uid=%u", cred->sc_uid);
	    rb_str_catf(ret, " euid=%u", cred->sc_euid);
	    rb_str_catf(ret, " gid=%u", cred->sc_gid);
	    rb_str_catf(ret, " egid=%u", cred->sc_egid);
	    if (cred0.sc_ngroups) {
		int i;
		const char *sep = " groups=";
		for (i = 0; i < cred0.sc_ngroups; i++) {
		    rb_str_catf(ret, "%s%u", sep, cred->sc_groups[i]);
		    sep = ",";
		}
	    }
	    rb_str_cat2(ret, " (sockcred)");
	    return 1;
	}
    }
#endif
    return 0;
}
#endif

#if defined(IPPROTO_IP) && defined(IP_RECVDSTADDR) /* 4.4BSD */
static int
anc_inspect_ip_recvdstaddr(int level, int type, VALUE data, VALUE ret)
{
    if (level == IPPROTO_IP && type == IP_RECVDSTADDR &&
        RSTRING_LEN(data) == sizeof(struct in_addr)) {
        struct in_addr addr;
        char addrbuf[INET_ADDRSTRLEN];
        memcpy(&addr, RSTRING_PTR(data), sizeof(addr));
        if (inet_ntop(AF_INET, &addr, addrbuf, (socklen_t)sizeof(addrbuf)) == NULL)
            rb_str_cat2(ret, " invalid-address");
        else
            rb_str_catf(ret, " %s", addrbuf);
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(IPPROTO_IP) && defined(IP_PKTINFO) && defined(HAVE_STRUCT_IN_PKTINFO_IPI_SPEC_DST) /* GNU/Linux */
static int
anc_inspect_ip_pktinfo(int level, int type, VALUE data, VALUE ret)
{
    if (level == IPPROTO_IP && type == IP_PKTINFO &&
        RSTRING_LEN(data) == sizeof(struct in_pktinfo)) {
        struct in_pktinfo pktinfo;
        char buf[INET_ADDRSTRLEN > IFNAMSIZ ? INET_ADDRSTRLEN : IFNAMSIZ];
        memcpy(&pktinfo, RSTRING_PTR(data), sizeof(pktinfo));
        if (inet_ntop(AF_INET, &pktinfo.ipi_addr, buf, sizeof(buf)) == NULL)
            rb_str_cat2(ret, " invalid-address");
        else
            rb_str_catf(ret, " %s", buf);
        if (if_indextoname(pktinfo.ipi_ifindex, buf) == NULL)
            rb_str_catf(ret, " ifindex:%d", pktinfo.ipi_ifindex);
        else
            rb_str_catf(ret, " %s", buf);
        if (inet_ntop(AF_INET, &pktinfo.ipi_spec_dst, buf, sizeof(buf)) == NULL)
            rb_str_cat2(ret, " spec_dst:invalid-address");
        else
            rb_str_catf(ret, " spec_dst:%s", buf);
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO) && defined(HAVE_TYPE_STRUCT_IN6_PKTINFO) /* IPv6 RFC3542 */
static int
anc_inspect_ipv6_pktinfo(int level, int type, VALUE data, VALUE ret)
{
    if (level == IPPROTO_IPV6 && type == IPV6_PKTINFO &&
        RSTRING_LEN(data) == sizeof(struct in6_pktinfo)) {
        struct in6_pktinfo *pktinfo = (struct in6_pktinfo *)RSTRING_PTR(data);
        struct in6_addr addr;
        unsigned int ifindex;
        char addrbuf[INET6_ADDRSTRLEN], ifbuf[IFNAMSIZ];
        memcpy(&addr, &pktinfo->ipi6_addr, sizeof(addr));
        memcpy(&ifindex, &pktinfo->ipi6_ifindex, sizeof(ifindex));
        if (inet_ntop(AF_INET6, &addr, addrbuf, (socklen_t)sizeof(addrbuf)) == NULL)
            rb_str_cat2(ret, " invalid-address");
        else
            rb_str_catf(ret, " %s", addrbuf);
        if (if_indextoname(ifindex, ifbuf) == NULL)
            rb_str_catf(ret, " ifindex:%d", ifindex);
        else
            rb_str_catf(ret, " %s", ifbuf);
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(SCM_TIMESTAMP) /* GNU/Linux, FreeBSD, NetBSD, OpenBSD, MacOS X, Solaris */
static int
inspect_timeval_as_abstime(int level, int optname, VALUE data, VALUE ret)
{
    if (RSTRING_LEN(data) == sizeof(struct timeval)) {
        struct timeval tv;
        time_t time;
        struct tm tm;
        char buf[32];
        memcpy((char*)&tv, RSTRING_PTR(data), sizeof(tv));
        time = tv.tv_sec;
        tm = *localtime(&time);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        rb_str_catf(ret, " %s.%06ld", buf, (long)tv.tv_usec);
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(SCM_TIMESTAMPNS) /* GNU/Linux */
static int
inspect_timespec_as_abstime(int level, int optname, VALUE data, VALUE ret)
{
    if (RSTRING_LEN(data) == sizeof(struct timespec)) {
        struct timespec ts;
        struct tm tm;
        char buf[32];
        memcpy((char*)&ts, RSTRING_PTR(data), sizeof(ts));
        tm = *localtime(&ts.tv_sec);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        rb_str_catf(ret, " %s.%09ld", buf, (long)ts.tv_nsec);
        return 1;
    }
    else {
        return 0;
    }
}
#endif

#if defined(SCM_BINTIME) /* FreeBSD */
static int
inspect_bintime_as_abstime(int level, int optname, VALUE data, VALUE ret)
{
    if (RSTRING_LEN(data) == sizeof(struct bintime)) {
        struct bintime bt;
        struct tm tm;
	uint64_t frac_h, frac_l;
	uint64_t scale_h, scale_l;
	uint64_t tmp1, tmp2;
	uint64_t res_h, res_l;
        char buf[32];
        memcpy((char*)&bt, RSTRING_PTR(data), sizeof(bt));
        tm = *localtime(&bt.sec);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);

	/* res_h = frac * 10**19 / 2**64 */

	frac_h = bt.frac >> 32;
	frac_l = bt.frac & 0xffffffff;

	scale_h = 0x8ac72304; /* 0x8ac7230489e80000 == 10**19 */
	scale_l = 0x89e80000;

	res_h = frac_h * scale_h;
	res_l = frac_l * scale_l;

	tmp1 = frac_h * scale_l;
	res_h += tmp1 >> 32;
	tmp2 = res_l;
	res_l += tmp1 & 0xffffffff;
	if (res_l < tmp2) res_h++;

	tmp1 = frac_l * scale_h;
	res_h += tmp1 >> 32;
	tmp2 = res_l;
	res_l += tmp1 & 0xffffffff;
	if (res_l < tmp2) res_h++;

        rb_str_catf(ret, " %s.%019"PRIu64, buf, res_h);
        return 1;
    }
    else {
        return 0;
    }
}
#endif

/*
 * call-seq:
 *   ancillarydata.inspect => string
 *
 * returns a string which shows ancillarydata in human-readable form.
 *
 *   p Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "").inspect
 *   #=> "#<Socket::AncillaryData: INET6 IPV6 PKTINFO \"\">"
 */
static VALUE
ancillary_inspect(VALUE self)
{
    VALUE ret;
    int family, level, type;
    VALUE data;
    ID family_id, level_id, type_id;
    VALUE vtype;
    int inspected;

    family = ancillary_family(self);
    level = ancillary_level(self);
    type = ancillary_type(self);
    data = ancillary_data(self);

    ret = rb_sprintf("#<%s:", rb_obj_classname(self));

    family_id = rsock_intern_family_noprefix(family);
    if (family_id)
        rb_str_catf(ret, " %s", rb_id2name(family_id));
    else
        rb_str_catf(ret, " family:%d", family);

    if (level == SOL_SOCKET) {
        rb_str_cat2(ret, " SOCKET");

        type_id = rsock_intern_scm_optname(type);
        if (type_id)
            rb_str_catf(ret, " %s", rb_id2name(type_id));
        else
            rb_str_catf(ret, " cmsg_type:%d", type);
    }
    else if (IS_IP_FAMILY(family)) {
        level_id = rsock_intern_iplevel(level);
        if (level_id)
            rb_str_catf(ret, " %s", rb_id2name(level_id));
        else
            rb_str_catf(ret, " cmsg_level:%d", level);

        vtype = ip_cmsg_type_to_sym(level, type);
        if (SYMBOL_P(vtype))
            rb_str_catf(ret, " %s", rb_id2name(SYM2ID(vtype)));
        else
            rb_str_catf(ret, " cmsg_type:%d", type);
    }
    else {
        rb_str_catf(ret, " cmsg_level:%d", level);
        rb_str_catf(ret, " cmsg_type:%d", type);
    }

    inspected = 0;

    if (level == SOL_SOCKET)
        family = AF_UNSPEC;

    switch (family) {
      case AF_UNSPEC:
        switch (level) {
#        if defined(SOL_SOCKET)
          case SOL_SOCKET:
            switch (type) {
#            if defined(SCM_TIMESTAMP) /* GNU/Linux, FreeBSD, NetBSD, OpenBSD, MacOS X, Solaris */
              case SCM_TIMESTAMP: inspected = inspect_timeval_as_abstime(level, type, data, ret); break;
#            endif
#            if defined(SCM_TIMESTAMPNS) /* GNU/Linux */
              case SCM_TIMESTAMPNS: inspected = inspect_timespec_as_abstime(level, type, data, ret); break;
#            endif
#            if defined(SCM_BINTIME) /* FreeBSD */
              case SCM_BINTIME: inspected = inspect_bintime_as_abstime(level, type, data, ret); break;
#            endif
#            if defined(SCM_RIGHTS) /* 4.4BSD */
              case SCM_RIGHTS: inspected = anc_inspect_socket_rights(level, type, data, ret); break;
#            endif
#            if defined(SCM_CREDENTIALS) /* GNU/Linux */
              case SCM_CREDENTIALS: inspected = anc_inspect_passcred_credentials(level, type, data, ret); break;
#            endif
#            if defined(INSPECT_SCM_CREDS) /* NetBSD */
              case SCM_CREDS: inspected = anc_inspect_socket_creds(level, type, data, ret); break;
#            endif
            }
            break;
#        endif
        }
        break;

      case AF_INET:
#ifdef INET6
      case AF_INET6:
#endif
        switch (level) {
#        if defined(IPPROTO_IP)
          case IPPROTO_IP:
            switch (type) {
#            if defined(IP_RECVDSTADDR) /* 4.4BSD */
              case IP_RECVDSTADDR: inspected = anc_inspect_ip_recvdstaddr(level, type, data, ret); break;
#            endif
#            if defined(IP_PKTINFO) && defined(HAVE_STRUCT_IN_PKTINFO_IPI_SPEC_DST) /* GNU/Linux */
              case IP_PKTINFO: inspected = anc_inspect_ip_pktinfo(level, type, data, ret); break;
#            endif
            }
            break;
#        endif

#        if defined(IPPROTO_IPV6)
          case IPPROTO_IPV6:
            switch (type) {
#            if defined(IPV6_PKTINFO) /* RFC 3542 */
              case IPV6_PKTINFO: inspected = anc_inspect_ipv6_pktinfo(level, type, data, ret); break;
#            endif
            }
            break;
#        endif
        }
        break;
    }

    if (!inspected) {
        rb_str_cat2(ret, " ");
        rb_str_append(ret, rb_str_dump(data));
    }

    rb_str_cat2(ret, ">");

    return ret;
}

/*
 * call-seq:
 *   ancillarydata.cmsg_is?(level, type) => true or false
 *
 * tests the level and type of _ancillarydata_.
 *
 *   ancdata = Socket::AncillaryData.new(:INET6, :IPV6, :PKTINFO, "")
 *   ancdata.cmsg_is?(Socket::IPPROTO_IPV6, Socket::IPV6_PKTINFO) #=> true
 *   ancdata.cmsg_is?(:IPV6, :PKTINFO)       #=> true
 *   ancdata.cmsg_is?(:IP, :PKTINFO)         #=> false
 *   ancdata.cmsg_is?(:SOCKET, :RIGHTS)      #=> false
 */
static VALUE
ancillary_cmsg_is_p(VALUE self, VALUE vlevel, VALUE vtype)
{
    int family = ancillary_family(self);
    int level = rsock_level_arg(family, vlevel);
    int type = rsock_cmsg_type_arg(family, level, vtype);

    if (ancillary_level(self) == level &&
        ancillary_type(self) == type)
        return Qtrue;
    else
        return Qfalse;
}

#endif

#if defined(HAVE_SENDMSG)
struct sendmsg_args_struct {
    int fd;
    const struct msghdr *msg;
    int flags;
};

static VALUE
nogvl_sendmsg_func(void *ptr)
{
    struct sendmsg_args_struct *args = ptr;
    return sendmsg(args->fd, args->msg, args->flags);
}

static ssize_t
rb_sendmsg(int fd, const struct msghdr *msg, int flags)
{
    struct sendmsg_args_struct args;
    args.fd = fd;
    args.msg = msg;
    args.flags = flags;
    return rb_thread_blocking_region(nogvl_sendmsg_func, &args, RUBY_UBF_IO, 0);
}

static VALUE
bsock_sendmsg_internal(int argc, VALUE *argv, VALUE sock, int nonblock)
{
    rb_io_t *fptr;
    VALUE data, vflags, dest_sockaddr;
    VALUE *controls_ptr;
    int controls_num;
    struct msghdr mh;
    struct iovec iov;
#if defined(HAVE_ST_MSG_CONTROL)
    volatile VALUE controls_str = 0;
#endif
    int flags;
    ssize_t ss;
    int family;

    rb_secure(4);
    GetOpenFile(sock, fptr);
    family = rsock_getfamily(fptr->fd);

    data = vflags = dest_sockaddr = Qnil;
    controls_ptr = NULL;
    controls_num = 0;

    if (argc == 0)
        rb_raise(rb_eArgError, "mesg argument required");
    data = argv[0];
    if (1 < argc) vflags = argv[1];
    if (2 < argc) dest_sockaddr = argv[2];
    if (3 < argc) { controls_ptr = &argv[3]; controls_num = argc - 3; }

    StringValue(data);

    if (controls_num) {
#if defined(HAVE_ST_MSG_CONTROL)
	int i;
	size_t last_pad = 0;
        int last_level = 0;
        int last_type = 0;
        controls_str = rb_str_tmp_new(0);
        for (i = 0; i < controls_num; i++) {
            VALUE elt = controls_ptr[i], v;
            VALUE vlevel, vtype;
            int level, type;
            VALUE cdata;
            long oldlen;
            struct cmsghdr cmh;
            char *cmsg;
            size_t cspace;
            v = rb_check_convert_type(elt, T_ARRAY, "Array", "to_ary");
            if (!NIL_P(v)) {
                elt = v;
                if (RARRAY_LEN(elt) != 3)
                    rb_raise(rb_eArgError, "an element of controls should be 3-elements array");
                vlevel = rb_ary_entry(elt, 0);
                vtype = rb_ary_entry(elt, 1);
                cdata = rb_ary_entry(elt, 2);
            }
            else {
                vlevel = rb_funcall(elt, rb_intern("level"), 0);
                vtype = rb_funcall(elt, rb_intern("type"), 0);
                cdata = rb_funcall(elt, rb_intern("data"), 0);
            }
            level = rsock_level_arg(family, vlevel);
            type = rsock_cmsg_type_arg(family, level, vtype);
            StringValue(cdata);
            oldlen = RSTRING_LEN(controls_str);
            cspace = CMSG_SPACE(RSTRING_LEN(cdata));
            rb_str_resize(controls_str, oldlen + cspace);
            cmsg = RSTRING_PTR(controls_str)+oldlen;
            memset((char *)cmsg, 0, cspace);
            memset((char *)&cmh, 0, sizeof(cmh));
            cmh.cmsg_level = level;
            cmh.cmsg_type = type;
            cmh.cmsg_len = (socklen_t)CMSG_LEN(RSTRING_LEN(cdata));
            MEMCPY(cmsg, &cmh, char, sizeof(cmh));
            MEMCPY(cmsg+((char*)CMSG_DATA(&cmh)-(char*)&cmh), RSTRING_PTR(cdata), char, RSTRING_LEN(cdata));
            last_level = cmh.cmsg_level;
            last_type = cmh.cmsg_type;
	    last_pad = cspace - cmh.cmsg_len;
        }
	if (last_pad) {
            /*
             * This code removes the last padding from msg_controllen.
             *
             * 4.3BSD-Reno reject the padding for SCM_RIGHTS. (There was no 64bit environments in those days?)
             * RFC 2292 require the padding.
             * RFC 3542 relaxes the condition - implementation must accept both as valid.
             *
             * Actual problems:
             *
             * - NetBSD 4.0.1
             *   SCM_RIGHTS with padding causes EINVAL
             *   IPV6_PKTINFO without padding causes "page fault trap"
             *     http://www.netbsd.org/cgi-bin/query-pr-single.pl?number=40661
             *
             * - OpenBSD 4.4
             *   IPV6_PKTINFO without padding causes EINVAL
             *
             * Basically, msg_controllen should contains the padding.
             * So the padding is removed only if a problem really exists.
             */
#if defined(__NetBSD__)
            if (last_level == SOL_SOCKET && last_type == SCM_RIGHTS)
                rb_str_set_len(controls_str, RSTRING_LEN(controls_str)-last_pad);
#endif
	}
#else
	rb_raise(rb_eNotImpError, "control message for sendmsg is unimplemented");
#endif
    }

    flags = NIL_P(vflags) ? 0 : NUM2INT(vflags);
#ifdef MSG_DONTWAIT
    if (nonblock)
        flags |= MSG_DONTWAIT;
#endif

    if (!NIL_P(dest_sockaddr))
	SockAddrStringValue(dest_sockaddr);

    rb_io_check_closed(fptr);

  retry:
    memset(&mh, 0, sizeof(mh));
    if (!NIL_P(dest_sockaddr)) {
        mh.msg_name = RSTRING_PTR(dest_sockaddr);
        mh.msg_namelen = RSTRING_LENINT(dest_sockaddr);
    }
    mh.msg_iovlen = 1;
    mh.msg_iov = &iov;
    iov.iov_base = RSTRING_PTR(data);
    iov.iov_len = RSTRING_LEN(data);
#if defined(HAVE_ST_MSG_CONTROL)
    if (controls_str) {
        mh.msg_control = RSTRING_PTR(controls_str);
        mh.msg_controllen = RSTRING_LENINT(controls_str);
    }
    else {
        mh.msg_control = NULL;
        mh.msg_controllen = 0;
    }
#endif

    rb_io_check_closed(fptr);
    if (nonblock)
        rb_io_set_nonblock(fptr);

    ss = rb_sendmsg(fptr->fd, &mh, flags);

    if (!nonblock && rb_io_wait_writable(fptr->fd)) {
        rb_io_check_closed(fptr);
        goto retry;
    }

    if (ss == -1) {
        if (nonblock && (errno == EWOULDBLOCK || errno == EAGAIN))
            rb_mod_sys_fail(rb_mWaitWritable, "sendmsg(2) would block");
	rb_sys_fail("sendmsg(2)");
    }

    return SSIZET2NUM(ss);
}
#endif

#if defined(HAVE_SENDMSG)
/*
 * call-seq:
 *    basicsocket.sendmsg(mesg, flags=0, dest_sockaddr=nil, *controls) => numbytes_sent
 *
 * sendmsg sends a message using sendmsg(2) system call in blocking manner.
 *
 * _mesg_ is a string to send.
 *
 * _flags_ is bitwise OR of MSG_* constants such as Socket::MSG_OOB.
 *
 * _dest_sockaddr_ is a destination socket address for connection-less socket.
 * It should be a sockaddr such as a result of Socket.sockaddr_in.
 * An Addrinfo object can be used too.
 *
 * _controls_ is a list of ancillary data.
 * The element of _controls_ should be Socket::AncillaryData or
 * 3-elements array.
 * The 3-element array should contains cmsg_level, cmsg_type and data.
 *
 * The return value, _numbytes_sent_ is an integer which is the number of bytes sent.
 *
 * sendmsg can be used to implement send_io as follows:
 *
 *   # use Socket::AncillaryData.
 *   ancdata = Socket::AncillaryData.int(:UNIX, :SOCKET, :RIGHTS, io.fileno)
 *   sock.sendmsg("a", 0, nil, ancdata)
 *
 *   # use 3-element array.
 *   ancdata = [:SOCKET, :RIGHTS, [io.fileno].pack("i!")]
 *   sock.sendmsg("\0", 0, nil, ancdata)
 *
 */
VALUE
rsock_bsock_sendmsg(int argc, VALUE *argv, VALUE sock)
{
    return bsock_sendmsg_internal(argc, argv, sock, 0);
}
#endif

#if defined(HAVE_SENDMSG)
/*
 * call-seq:
 *    basicsocket.sendmsg_nonblock(mesg, flags=0, dest_sockaddr=nil, *controls) => numbytes_sent
 *
 * sendmsg_nonblock sends a message using sendmsg(2) system call in non-blocking manner.
 *
 * It is similar to BasicSocket#sendmsg
 * but the non-blocking flag is set before the system call
 * and it doesn't retry the system call.
 *
 */
VALUE
rsock_bsock_sendmsg_nonblock(int argc, VALUE *argv, VALUE sock)
{
    return bsock_sendmsg_internal(argc, argv, sock, 1);
}
#endif

#if defined(HAVE_RECVMSG)
struct recvmsg_args_struct {
    int fd;
    struct msghdr *msg;
    int flags;
};

ssize_t
rsock_recvmsg(int socket, struct msghdr *message, int flags)
{
#ifdef MSG_CMSG_CLOEXEC
    /* MSG_CMSG_CLOEXEC is available since Linux 2.6.23.  Linux 2.6.18 silently ignore it. */
    flags |= MSG_CMSG_CLOEXEC;
#endif
    return recvmsg(socket, message, flags);
}

static VALUE
nogvl_recvmsg_func(void *ptr)
{
    struct recvmsg_args_struct *args = ptr;
    int flags = args->flags;
    return rsock_recvmsg(args->fd, args->msg, flags);
}

static ssize_t
rb_recvmsg(int fd, struct msghdr *msg, int flags)
{
    struct recvmsg_args_struct args;
    args.fd = fd;
    args.msg = msg;
    args.flags = flags;
    return rb_thread_blocking_region(nogvl_recvmsg_func, &args, RUBY_UBF_IO, 0);
}

#if defined(HAVE_ST_MSG_CONTROL)
static void
discard_cmsg(struct cmsghdr *cmh, char *msg_end, int msg_peek_p)
{
# if !defined(FD_PASSING_WORK_WITH_RECVMSG_MSG_PEEK)
    /*
     * FreeBSD 8.2.0, NetBSD 5 and MacOS X Snow Leopard doesn't
     * allocate fds by recvmsg with MSG_PEEK.
     * [ruby-dev:44189]
     * http://bugs.ruby-lang.org/issues/5075
     *
     * Linux 2.6.38 allocate fds by recvmsg with MSG_PEEK.
     */
    if (msg_peek_p)
        return;
# endif
    if (cmh->cmsg_level == SOL_SOCKET && cmh->cmsg_type == SCM_RIGHTS) {
        int *fdp = (int *)CMSG_DATA(cmh);
        int *end = (int *)((char *)cmh + cmh->cmsg_len);
        while ((char *)fdp + sizeof(int) <= (char *)end &&
               (char *)fdp + sizeof(int) <= msg_end) {
            rb_fd_fix_cloexec(*fdp);
            close(*fdp);
            fdp++;
        }
    }
}
#endif

void
rsock_discard_cmsg_resource(struct msghdr *mh, int msg_peek_p)
{
#if defined(HAVE_ST_MSG_CONTROL)
    struct cmsghdr *cmh;
    char *msg_end;

    if (mh->msg_controllen == 0)
        return;

    msg_end = (char *)mh->msg_control + mh->msg_controllen;

    for (cmh = CMSG_FIRSTHDR(mh); cmh != NULL; cmh = CMSG_NXTHDR(mh, cmh)) {
        discard_cmsg(cmh, msg_end, msg_peek_p);
    }
#endif
}

#if defined(HAVE_ST_MSG_CONTROL)
static void
make_io_for_unix_rights(VALUE ctl, struct cmsghdr *cmh, char *msg_end)
{
    if (cmh->cmsg_level == SOL_SOCKET && cmh->cmsg_type == SCM_RIGHTS) {
        int *fdp, *end;
	VALUE ary = rb_ary_new();
	rb_ivar_set(ctl, rb_intern("unix_rights"), ary);
        fdp = (int *)CMSG_DATA(cmh);
        end = (int *)((char *)cmh + cmh->cmsg_len);
        while ((char *)fdp + sizeof(int) <= (char *)end &&
	       (char *)fdp + sizeof(int) <= msg_end) {
            int fd = *fdp;
            struct stat stbuf;
            VALUE io;
            if (fstat(fd, &stbuf) == -1)
                rb_raise(rb_eSocket, "invalid fd in SCM_RIGHTS");
            rb_fd_fix_cloexec(fd);
            if (S_ISSOCK(stbuf.st_mode))
                io = rsock_init_sock(rb_obj_alloc(rb_cSocket), fd);
            else
                io = rb_io_fdopen(fd, O_RDWR, NULL);
            ary = rb_attr_get(ctl, rb_intern("unix_rights"));
            rb_ary_push(ary, io);
            fdp++;
        }
	OBJ_FREEZE(ary);
    }
}
#endif

static VALUE
bsock_recvmsg_internal(int argc, VALUE *argv, VALUE sock, int nonblock)
{
    rb_io_t *fptr;
    VALUE vmaxdatlen, vmaxctllen, vflags, vopts;
    int grow_buffer;
    size_t maxdatlen;
    int flags, orig_flags;
    int request_scm_rights;
    struct msghdr mh;
    struct iovec iov;
    struct sockaddr_storage namebuf;
    char datbuf0[4096], *datbuf;
    VALUE dat_str = Qnil;
    VALUE ret;
    ssize_t ss;
#if defined(HAVE_ST_MSG_CONTROL)
    struct cmsghdr *cmh;
    size_t maxctllen;
    union {
        char bytes[4096];
        struct cmsghdr align;
    } ctlbuf0;
    char *ctlbuf;
    VALUE ctl_str = Qnil;
    int family;
    int gc_done = 0;
#endif

    rb_secure(4);

    vopts = Qnil;
    if (0 < argc && TYPE(argv[argc-1]) == T_HASH)
        vopts = argv[--argc];

    rb_scan_args(argc, argv, "03", &vmaxdatlen, &vflags, &vmaxctllen);

    maxdatlen = NIL_P(vmaxdatlen) ? sizeof(datbuf0) : NUM2SIZET(vmaxdatlen);
#if defined(HAVE_ST_MSG_CONTROL)
    maxctllen = NIL_P(vmaxctllen) ? sizeof(ctlbuf0) : NUM2SIZET(vmaxctllen);
#else
    if (!NIL_P(vmaxctllen))
        rb_raise(rb_eArgError, "control message not supported");
#endif
    flags = NIL_P(vflags) ? 0 : NUM2INT(vflags);
#ifdef MSG_DONTWAIT
    if (nonblock)
        flags |= MSG_DONTWAIT;
#endif
    orig_flags = flags;

    grow_buffer = NIL_P(vmaxdatlen) || NIL_P(vmaxctllen);

    request_scm_rights = 0;
    if (!NIL_P(vopts) && RTEST(rb_hash_aref(vopts, ID2SYM(rb_intern("scm_rights")))))
        request_scm_rights = 1;

    GetOpenFile(sock, fptr);
    if (rb_io_read_pending(fptr)) {
        rb_raise(rb_eIOError, "recvmsg for buffered IO");
    }

#if !defined(HAVE_ST_MSG_CONTROL)
    if (grow_buffer) {
	int socktype;
	socklen_t optlen = (socklen_t)sizeof(socktype);
        if (getsockopt(fptr->fd, SOL_SOCKET, SO_TYPE, (void*)&socktype, &optlen) == -1) {
	    rb_sys_fail("getsockopt(SO_TYPE)");
	}
	if (socktype == SOCK_STREAM)
	    grow_buffer = 0;
    }
#endif

  retry:
    if (maxdatlen <= sizeof(datbuf0))
        datbuf = datbuf0;
    else {
        if (NIL_P(dat_str))
            dat_str = rb_str_tmp_new(maxdatlen);
        else
            rb_str_resize(dat_str, maxdatlen);
        datbuf = RSTRING_PTR(dat_str);
    }

#if defined(HAVE_ST_MSG_CONTROL)
    if (maxctllen <= sizeof(ctlbuf0))
        ctlbuf = ctlbuf0.bytes;
    else {
        if (NIL_P(ctl_str))
            ctl_str = rb_str_tmp_new(maxctllen);
        else
            rb_str_resize(ctl_str, maxctllen);
        ctlbuf = RSTRING_PTR(ctl_str);
    }
#endif

    memset(&mh, 0, sizeof(mh));

    memset(&namebuf, 0, sizeof(namebuf));
    mh.msg_name = (struct sockaddr *)&namebuf;
    mh.msg_namelen = (socklen_t)sizeof(namebuf);

    mh.msg_iov = &iov;
    mh.msg_iovlen = 1;
    iov.iov_base = datbuf;
    iov.iov_len = maxdatlen;

#if defined(HAVE_ST_MSG_CONTROL)
    mh.msg_control = ctlbuf;
    mh.msg_controllen = (socklen_t)maxctllen;
#endif

    if (grow_buffer)
        flags |= MSG_PEEK;

    rb_io_check_closed(fptr);
    if (nonblock)
        rb_io_set_nonblock(fptr);

    ss = rb_recvmsg(fptr->fd, &mh, flags);

    if (!nonblock && rb_io_wait_readable(fptr->fd)) {
        rb_io_check_closed(fptr);
        goto retry;
    }

    if (ss == -1) {
        if (nonblock && (errno == EWOULDBLOCK || errno == EAGAIN))
            rb_mod_sys_fail(rb_mWaitReadable, "recvmsg(2) would block");
#if defined(HAVE_ST_MSG_CONTROL)
        if (!gc_done && (errno == EMFILE || errno == EMSGSIZE)) {
          /*
           * When SCM_RIGHTS hit the file descriptors limit:
           * - Linux 2.6.18 causes success with MSG_CTRUNC
           * - MacOS X 10.4 causes EMSGSIZE (and lost file descriptors?)
           * - Solaris 11 causes EMFILE
           */
          gc_and_retry:
            rb_gc();
            gc_done = 1;
	    goto retry;
        }
#endif
	rb_sys_fail("recvmsg(2)");
    }

    if (grow_buffer) {
	int grown = 0;
#if defined(HAVE_ST_MSG_CONTROL)
        if (NIL_P(vmaxdatlen) && (mh.msg_flags & MSG_TRUNC)) {
            if (SIZE_MAX/2 < maxdatlen)
                rb_raise(rb_eArgError, "max data length too big");
	    maxdatlen *= 2;
	    grown = 1;
	}
        if (NIL_P(vmaxctllen) && (mh.msg_flags & MSG_CTRUNC)) {
#define BIG_ENOUGH_SPACE 65536
            if (BIG_ENOUGH_SPACE < maxctllen &&
                mh.msg_controllen < (socklen_t)(maxctllen - BIG_ENOUGH_SPACE)) {
                /* there are big space bug truncated.
                 * file descriptors limit? */
                if (!gc_done) {
		    rsock_discard_cmsg_resource(&mh, (flags & MSG_PEEK) != 0);
                    goto gc_and_retry;
		}
            }
            else {
                if (SIZE_MAX/2 < maxctllen)
                    rb_raise(rb_eArgError, "max control message length too big");
                maxctllen *= 2;
                grown = 1;
            }
#undef BIG_ENOUGH_SPACE
	}
#else
	if (NIL_P(vmaxdatlen) && ss != -1 && ss == (ssize_t)iov.iov_len) {
            if (SIZE_MAX/2 < maxdatlen)
                rb_raise(rb_eArgError, "max data length too big");
	    maxdatlen *= 2;
	    grown = 1;
	}
#endif
	if (grown) {
            rsock_discard_cmsg_resource(&mh, (flags & MSG_PEEK) != 0);
	    goto retry;
	}
	else {
            grow_buffer = 0;
            if (flags != orig_flags) {
                rsock_discard_cmsg_resource(&mh, (flags & MSG_PEEK) != 0);
                flags = orig_flags;
                goto retry;
            }
        }
    }

    if (NIL_P(dat_str))
        dat_str = rb_tainted_str_new(datbuf, ss);
    else {
        rb_str_resize(dat_str, ss);
        OBJ_TAINT(dat_str);
        RBASIC(dat_str)->klass = rb_cString;
    }

    ret = rb_ary_new3(3, dat_str,
                         rsock_io_socket_addrinfo(sock, mh.msg_name, mh.msg_namelen),
#if defined(HAVE_ST_MSG_CONTROL)
			 INT2NUM(mh.msg_flags)
#else
			 Qnil
#endif
			 );

#if defined(HAVE_ST_MSG_CONTROL)
    family = rsock_getfamily(fptr->fd);
    if (mh.msg_controllen) {
	char *msg_end = (char *)mh.msg_control + mh.msg_controllen;
        for (cmh = CMSG_FIRSTHDR(&mh); cmh != NULL; cmh = CMSG_NXTHDR(&mh, cmh)) {
            VALUE ctl;
	    char *ctl_end;
            size_t clen;
            if (cmh->cmsg_len == 0) {
                rb_raise(rb_eTypeError, "invalid control message (cmsg_len == 0)");
            }
            ctl_end = (char*)cmh + cmh->cmsg_len;
	    clen = (ctl_end <= msg_end ? ctl_end : msg_end) - (char*)CMSG_DATA(cmh);
            ctl = ancdata_new(family, cmh->cmsg_level, cmh->cmsg_type, rb_tainted_str_new((char*)CMSG_DATA(cmh), clen));
            if (request_scm_rights)
                make_io_for_unix_rights(ctl, cmh, msg_end);
            else
                discard_cmsg(cmh, msg_end, (flags & MSG_PEEK) != 0);
            rb_ary_push(ret, ctl);
        }
    }
#endif

    return ret;
}
#endif

#if defined(HAVE_RECVMSG)
/*
 * call-seq:
 *    basicsocket.recvmsg(maxmesglen=nil, flags=0, maxcontrollen=nil, opts={}) => [mesg, sender_addrinfo, rflags, *controls]
 *
 * recvmsg receives a message using recvmsg(2) system call in blocking manner.
 *
 * _maxmesglen_ is the maximum length of mesg to receive.
 *
 * _flags_ is bitwise OR of MSG_* constants such as Socket::MSG_PEEK.
 *
 * _maxcontrollen_ is the maximum length of controls (ancillary data) to receive.
 *
 * _opts_ is option hash.
 * Currently :scm_rights=>bool is the only option.
 *
 * :scm_rights option specifies that application expects SCM_RIGHTS control message.
 * If the value is nil or false, application don't expects SCM_RIGHTS control message.
 * In this case, recvmsg closes the passed file descriptors immediately.
 * This is the default behavior.
 *
 * If :scm_rights value is neither nil nor false, application expects SCM_RIGHTS control message.
 * In this case, recvmsg creates IO objects for each file descriptors for
 * Socket::AncillaryData#unix_rights method.
 *
 * The return value is 4-elements array.
 *
 * _mesg_ is a string of the received message.
 *
 * _sender_addrinfo_ is a sender socket address for connection-less socket.
 * It is an Addrinfo object.
 * For connection-oriented socket such as TCP, sender_addrinfo is platform dependent.
 *
 * _rflags_ is a flags on the received message which is bitwise OR of MSG_* constants such as Socket::MSG_TRUNC.
 * It will be nil if the system uses 4.3BSD style old recvmsg system call.
 *
 * _controls_ is ancillary data which is an array of Socket::AncillaryData objects such as:
 *
 *   #<Socket::AncillaryData: AF_UNIX SOCKET RIGHTS 7>
 *
 * _maxmesglen_ and _maxcontrollen_ can be nil.
 * In that case, the buffer will be grown until the message is not truncated.
 * Internally, MSG_PEEK is used and MSG_TRUNC/MSG_CTRUNC are checked.
 *
 * recvmsg can be used to implement recv_io as follows:
 *
 *   mesg, sender_sockaddr, rflags, *controls = sock.recvmsg(:scm_rights=>true)
 *   controls.each {|ancdata|
 *     if ancdata.cmsg_is?(:SOCKET, :RIGHTS)
 *       return ancdata.unix_rights[0]
 *     end
 *   }
 *
 */
VALUE
rsock_bsock_recvmsg(int argc, VALUE *argv, VALUE sock)
{
    return bsock_recvmsg_internal(argc, argv, sock, 0);
}
#endif

#if defined(HAVE_RECVMSG)
/*
 * call-seq:
 *    basicsocket.recvmsg_nonblock(maxdatalen=nil, flags=0, maxcontrollen=nil, opts={}) => [data, sender_addrinfo, rflags, *controls]
 *
 * recvmsg receives a message using recvmsg(2) system call in non-blocking manner.
 *
 * It is similar to BasicSocket#recvmsg
 * but non-blocking flag is set before the system call
 * and it doesn't retry the system call.
 *
 */
VALUE
rsock_bsock_recvmsg_nonblock(int argc, VALUE *argv, VALUE sock)
{
    return bsock_recvmsg_internal(argc, argv, sock, 1);
}
#endif

void
rsock_init_ancdata(void)
{
#if defined(HAVE_ST_MSG_CONTROL)
    /*
     * Document-class: Socket::AncillaryData
     *
     * Socket::AncillaryData represents the ancillary data (control information)
     * used by sendmsg and recvmsg system call.  It contains socket #family,
     * control message (cmsg) #level, cmsg #type and cmsg #data.
     */
    rb_cAncillaryData = rb_define_class_under(rb_cSocket, "AncillaryData", rb_cObject);
    rb_define_method(rb_cAncillaryData, "initialize", ancillary_initialize, 4);
    rb_define_method(rb_cAncillaryData, "inspect", ancillary_inspect, 0);
    rb_define_method(rb_cAncillaryData, "family", ancillary_family_m, 0);
    rb_define_method(rb_cAncillaryData, "level", ancillary_level_m, 0);
    rb_define_method(rb_cAncillaryData, "type", ancillary_type_m, 0);
    rb_define_method(rb_cAncillaryData, "data", ancillary_data, 0);

    rb_define_method(rb_cAncillaryData, "cmsg_is?", ancillary_cmsg_is_p, 2);

    rb_define_singleton_method(rb_cAncillaryData, "int", ancillary_s_int, 4);
    rb_define_method(rb_cAncillaryData, "int", ancillary_int, 0);

    rb_define_singleton_method(rb_cAncillaryData, "unix_rights", ancillary_s_unix_rights, -1);
    rb_define_method(rb_cAncillaryData, "unix_rights", ancillary_unix_rights, 0);

    rb_define_method(rb_cAncillaryData, "timestamp", ancillary_timestamp, 0);

    rb_define_singleton_method(rb_cAncillaryData, "ip_pktinfo", ancillary_s_ip_pktinfo, -1);
    rb_define_method(rb_cAncillaryData, "ip_pktinfo", ancillary_ip_pktinfo, 0);

    rb_define_singleton_method(rb_cAncillaryData, "ipv6_pktinfo", ancillary_s_ipv6_pktinfo, 2);
    rb_define_method(rb_cAncillaryData, "ipv6_pktinfo", ancillary_ipv6_pktinfo, 0);
    rb_define_method(rb_cAncillaryData, "ipv6_pktinfo_addr", ancillary_ipv6_pktinfo_addr, 0);
    rb_define_method(rb_cAncillaryData, "ipv6_pktinfo_ifindex", ancillary_ipv6_pktinfo_ifindex, 0);
#endif
}
