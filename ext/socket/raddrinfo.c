/************************************************

  ainfo.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

#if defined(INET6) && (defined(LOOKUP_ORDER_HACK_INET) || defined(LOOKUP_ORDER_HACK_INET6))
#define LOOKUP_ORDERS (sizeof(lookup_order_table) / sizeof(lookup_order_table[0]))
static const int lookup_order_table[] = {
#if defined(LOOKUP_ORDER_HACK_INET)
    PF_INET, PF_INET6, PF_UNSPEC,
#elif defined(LOOKUP_ORDER_HACK_INET6)
    PF_INET6, PF_INET, PF_UNSPEC,
#else
    /* should not happen */
#endif
};

static int
ruby_getaddrinfo(const char *nodename, const char *servname,
		 const struct addrinfo *hints, struct addrinfo **res)
{
    struct addrinfo tmp_hints;
    int i, af, error;

    if (hints->ai_family != PF_UNSPEC) {
	return getaddrinfo(nodename, servname, hints, res);
    }

    for (i = 0; i < LOOKUP_ORDERS; i++) {
	af = lookup_order_table[i];
	MEMCPY(&tmp_hints, hints, struct addrinfo, 1);
	tmp_hints.ai_family = af;
	error = getaddrinfo(nodename, servname, &tmp_hints, res);
	if (error) {
	    if (tmp_hints.ai_family == PF_UNSPEC) {
		break;
	    }
	}
	else {
	    break;
	}
    }

    return error;
}
#define getaddrinfo(node,serv,hints,res) ruby_getaddrinfo((node),(serv),(hints),(res))
#endif

#if defined(_AIX)
static int
ruby_getaddrinfo__aix(const char *nodename, const char *servname,
		      const struct addrinfo *hints, struct addrinfo **res)
{
    int error = getaddrinfo(nodename, servname, hints, res);
    struct addrinfo *r;
    if (error)
	return error;
    for (r = *res; r != NULL; r = r->ai_next) {
	if (r->ai_addr->sa_family == 0)
	    r->ai_addr->sa_family = r->ai_family;
	if (r->ai_addr->sa_len == 0)
	    r->ai_addr->sa_len = r->ai_addrlen;
    }
    return 0;
}
#undef getaddrinfo
#define getaddrinfo(node,serv,hints,res) ruby_getaddrinfo__aix((node),(serv),(hints),(res))
static int
ruby_getnameinfo__aix(const struct sockaddr *sa, size_t salen,
		      char *host, size_t hostlen,
		      char *serv, size_t servlen, int flags)
{
    struct sockaddr_in6 *sa6;
    u_int32_t *a6;

    if (sa->sa_family == AF_INET6) {
	sa6 = (struct sockaddr_in6 *)sa;
	a6 = sa6->sin6_addr.u6_addr.u6_addr32;

	if (a6[0] == 0 && a6[1] == 0 && a6[2] == 0 && a6[3] == 0) {
	    strncpy(host, "::", hostlen);
	    snprintf(serv, servlen, "%d", sa6->sin6_port);
	    return 0;
	}
    }
    return getnameinfo(sa, salen, host, hostlen, serv, servlen, flags);
}
#undef getnameinfo
#define getnameinfo(sa, salen, host, hostlen, serv, servlen, flags) \
            ruby_getnameinfo__aix((sa), (salen), (host), (hostlen), (serv), (servlen), (flags))
#endif

static int str_is_number(const char *);

#if defined(__APPLE__)
static int
ruby_getaddrinfo__darwin(const char *nodename, const char *servname,
			 const struct addrinfo *hints, struct addrinfo **res)
{
    /* fix [ruby-core:29427] */
    const char *tmp_servname;
    struct addrinfo tmp_hints;
    int error;

    tmp_servname = servname;
    MEMCPY(&tmp_hints, hints, struct addrinfo, 1);
    if (nodename && servname) {
	if (str_is_number(tmp_servname) && atoi(servname) == 0) {
	    tmp_servname = NULL;
#ifdef AI_NUMERICSERV
	    if (tmp_hints.ai_flags) tmp_hints.ai_flags &= ~AI_NUMERICSERV;
#endif
	}
    }

    error = getaddrinfo(nodename, tmp_servname, &tmp_hints, res);
    if (error == 0) {
        /* [ruby-dev:23164] */
        struct addrinfo *r;
        r = *res;
        while (r) {
            if (! r->ai_socktype) r->ai_socktype = hints->ai_socktype;
            if (! r->ai_protocol) {
                if (r->ai_socktype == SOCK_DGRAM) {
                    r->ai_protocol = IPPROTO_UDP;
                }
                else if (r->ai_socktype == SOCK_STREAM) {
                    r->ai_protocol = IPPROTO_TCP;
                }
            }
            r = r->ai_next;
        }
    }

    return error;
}
#undef getaddrinfo
#define getaddrinfo(node,serv,hints,res) ruby_getaddrinfo__darwin((node),(serv),(hints),(res))
#endif

#ifndef GETADDRINFO_EMU
struct getaddrinfo_arg
{
    const char *node;
    const char *service;
    const struct addrinfo *hints;
    struct addrinfo **res;
};

static VALUE
nogvl_getaddrinfo(void *arg)
{
    struct getaddrinfo_arg *ptr = arg;
    return getaddrinfo(ptr->node, ptr->service,
                       ptr->hints, ptr->res);
}
#endif

int
rb_getaddrinfo(const char *node, const char *service,
               const struct addrinfo *hints,
               struct addrinfo **res)
{
#ifdef GETADDRINFO_EMU
    return getaddrinfo(node, service, hints, res);
#else
    struct getaddrinfo_arg arg;
    int ret;
    MEMZERO(&arg, sizeof arg, 1);
    arg.node = node;
    arg.service = service;
    arg.hints = hints;
    arg.res = res;
    ret = (int)BLOCKING_REGION(nogvl_getaddrinfo, &arg);
    return ret;
#endif
}

#ifndef GETADDRINFO_EMU
struct getnameinfo_arg
{
    const struct sockaddr *sa;
    socklen_t salen;
    char *host;
    size_t hostlen;
    char *serv;
    size_t servlen;
    int flags;
};

static VALUE
nogvl_getnameinfo(void *arg)
{
    struct getnameinfo_arg *ptr = arg;
    return getnameinfo(ptr->sa, ptr->salen,
                       ptr->host, (socklen_t)ptr->hostlen,
                       ptr->serv, (socklen_t)ptr->servlen,
                       ptr->flags);
}
#endif

int
rb_getnameinfo(const struct sockaddr *sa, socklen_t salen,
           char *host, size_t hostlen,
           char *serv, size_t servlen, int flags)
{
#ifdef GETADDRINFO_EMU
    return getnameinfo(sa, salen, host, hostlen, serv, servlen, flags);
#else
    struct getnameinfo_arg arg;
    int ret;
    arg.sa = sa;
    arg.salen = salen;
    arg.host = host;
    arg.hostlen = hostlen;
    arg.serv = serv;
    arg.servlen = servlen;
    arg.flags = flags;
    ret = (int)BLOCKING_REGION(nogvl_getnameinfo, &arg);
    return ret;
#endif
}

static void
make_ipaddr0(struct sockaddr *addr, char *buf, size_t len)
{
    int error;

    error = rb_getnameinfo(addr, SA_LEN(addr), buf, len, NULL, 0, NI_NUMERICHOST);
    if (error) {
        rsock_raise_socket_error("getnameinfo", error);
    }
}

VALUE
rsock_make_ipaddr(struct sockaddr *addr)
{
    char hbuf[1024];

    make_ipaddr0(addr, hbuf, sizeof(hbuf));
    return rb_str_new2(hbuf);
}

static void
make_inetaddr(unsigned int host, char *buf, size_t len)
{
    struct sockaddr_in sin;

    MEMZERO(&sin, struct sockaddr_in, 1);
    sin.sin_family = AF_INET;
    SET_SIN_LEN(&sin, sizeof(sin));
    sin.sin_addr.s_addr = host;
    make_ipaddr0((struct sockaddr*)&sin, buf, len);
}

static int
str_is_number(const char *p)
{
    char *ep;

    if (!p || *p == '\0')
       return 0;
    ep = NULL;
    (void)STRTOUL(p, &ep, 10);
    if (ep && *ep == '\0')
       return 1;
    else
       return 0;
}

static char*
host_str(VALUE host, char *hbuf, size_t len, int *flags_ptr)
{
    if (NIL_P(host)) {
        return NULL;
    }
    else if (rb_obj_is_kind_of(host, rb_cInteger)) {
        unsigned int i = NUM2UINT(host);

        make_inetaddr(htonl(i), hbuf, len);
        if (flags_ptr) *flags_ptr |= AI_NUMERICHOST;
        return hbuf;
    }
    else {
        char *name;

        SafeStringValue(host);
        name = RSTRING_PTR(host);
        if (!name || *name == 0 || (name[0] == '<' && strcmp(name, "<any>") == 0)) {
            make_inetaddr(INADDR_ANY, hbuf, len);
            if (flags_ptr) *flags_ptr |= AI_NUMERICHOST;
        }
        else if (name[0] == '<' && strcmp(name, "<broadcast>") == 0) {
            make_inetaddr(INADDR_BROADCAST, hbuf, len);
            if (flags_ptr) *flags_ptr |= AI_NUMERICHOST;
        }
        else if (strlen(name) >= len) {
            rb_raise(rb_eArgError, "hostname too long (%"PRIuSIZE")",
                strlen(name));
        }
        else {
            strcpy(hbuf, name);
        }
        return hbuf;
    }
}

static char*
port_str(VALUE port, char *pbuf, size_t len, int *flags_ptr)
{
    if (NIL_P(port)) {
        return 0;
    }
    else if (FIXNUM_P(port)) {
        snprintf(pbuf, len, "%ld", FIX2LONG(port));
#ifdef AI_NUMERICSERV
        if (flags_ptr) *flags_ptr |= AI_NUMERICSERV;
#endif
        return pbuf;
    }
    else {
        char *serv;

        SafeStringValue(port);
        serv = RSTRING_PTR(port);
        if (strlen(serv) >= len) {
            rb_raise(rb_eArgError, "service name too long (%"PRIuSIZE")",
                strlen(serv));
        }
        strcpy(pbuf, serv);
        return pbuf;
    }
}

struct addrinfo*
rsock_getaddrinfo(VALUE host, VALUE port, struct addrinfo *hints, int socktype_hack)
{
    struct addrinfo* res = NULL;
    char *hostp, *portp;
    int error;
    char hbuf[NI_MAXHOST], pbuf[NI_MAXSERV];
    int additional_flags = 0;

    hostp = host_str(host, hbuf, sizeof(hbuf), &additional_flags);
    portp = port_str(port, pbuf, sizeof(pbuf), &additional_flags);

    if (socktype_hack && hints->ai_socktype == 0 && str_is_number(portp)) {
       hints->ai_socktype = SOCK_DGRAM;
    }
    hints->ai_flags |= additional_flags;

    error = rb_getaddrinfo(hostp, portp, hints, &res);
    if (error) {
        if (hostp && hostp[strlen(hostp)-1] == '\n') {
            rb_raise(rb_eSocket, "newline at the end of hostname");
        }
        rsock_raise_socket_error("getaddrinfo", error);
    }

    return res;
}

struct addrinfo*
rsock_addrinfo(VALUE host, VALUE port, int socktype, int flags)
{
    struct addrinfo hints;

    MEMZERO(&hints, struct addrinfo, 1);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = socktype;
    hints.ai_flags = flags;
    return rsock_getaddrinfo(host, port, &hints, 1);
}

VALUE
rsock_ipaddr(struct sockaddr *sockaddr, int norevlookup)
{
    VALUE family, port, addr1, addr2;
    VALUE ary;
    int error;
    char hbuf[1024], pbuf[1024];
    ID id;

    id = rsock_intern_family(sockaddr->sa_family);
    if (id) {
        family = rb_str_dup(rb_id2str(id));
    }
    else {
        sprintf(pbuf, "unknown:%d", sockaddr->sa_family);
        family = rb_str_new2(pbuf);
    }

    addr1 = Qnil;
    if (!norevlookup) {
        error = rb_getnameinfo(sockaddr, SA_LEN(sockaddr), hbuf, sizeof(hbuf),
                               NULL, 0, 0);
        if (! error) {
            addr1 = rb_str_new2(hbuf);
        }
    }
    error = rb_getnameinfo(sockaddr, SA_LEN(sockaddr), hbuf, sizeof(hbuf),
                           pbuf, sizeof(pbuf), NI_NUMERICHOST | NI_NUMERICSERV);
    if (error) {
        rsock_raise_socket_error("getnameinfo", error);
    }
    addr2 = rb_str_new2(hbuf);
    if (addr1 == Qnil) {
        addr1 = addr2;
    }
    port = INT2FIX(atoi(pbuf));
    ary = rb_ary_new3(4, family, port, addr1, addr2);

    return ary;
}

#ifdef HAVE_SYS_UN_H
const char*
rsock_unixpath(struct sockaddr_un *sockaddr, socklen_t len)
{
    if (sockaddr->sun_path < (char*)sockaddr + len)
        return sockaddr->sun_path;
    else
        return "";
}

VALUE
rsock_unixaddr(struct sockaddr_un *sockaddr, socklen_t len)
{
    return rb_assoc_new(rb_str_new2("AF_UNIX"),
                        rb_str_new2(rsock_unixpath(sockaddr, len)));
}
#endif

struct hostent_arg {
    VALUE host;
    struct addrinfo* addr;
    VALUE (*ipaddr)(struct sockaddr*, size_t);
};

static VALUE
make_hostent_internal(struct hostent_arg *arg)
{
    VALUE host = arg->host;
    struct addrinfo* addr = arg->addr;
    VALUE (*ipaddr)(struct sockaddr*, size_t) = arg->ipaddr;

    struct addrinfo *ai;
    struct hostent *h;
    VALUE ary, names;
    char **pch;
    const char* hostp;
    char hbuf[NI_MAXHOST];

    ary = rb_ary_new();
    if (addr->ai_canonname) {
        hostp = addr->ai_canonname;
    }
    else {
        hostp = host_str(host, hbuf, sizeof(hbuf), NULL);
    }
    rb_ary_push(ary, rb_str_new2(hostp));

    if (addr->ai_canonname && (h = gethostbyname(addr->ai_canonname))) {
        names = rb_ary_new();
        if (h->h_aliases != NULL) {
            for (pch = h->h_aliases; *pch; pch++) {
                rb_ary_push(names, rb_str_new2(*pch));
            }
        }
    }
    else {
        names = rb_ary_new2(0);
    }
    rb_ary_push(ary, names);
    rb_ary_push(ary, INT2NUM(addr->ai_family));
    for (ai = addr; ai; ai = ai->ai_next) {
        rb_ary_push(ary, (*ipaddr)(ai->ai_addr, ai->ai_addrlen));
    }

    return ary;
}

VALUE
rsock_freeaddrinfo(struct addrinfo *addr)
{
    freeaddrinfo(addr);
    return Qnil;
}

VALUE
rsock_make_hostent(VALUE host, struct addrinfo *addr, VALUE (*ipaddr)(struct sockaddr *, size_t))
{
    struct hostent_arg arg;

    arg.host = host;
    arg.addr = addr;
    arg.ipaddr = ipaddr;
    return rb_ensure(make_hostent_internal, (VALUE)&arg,
                     rsock_freeaddrinfo, (VALUE)addr);
}

typedef struct {
    VALUE inspectname;
    VALUE canonname;
    int pfamily;
    int socktype;
    int protocol;
    socklen_t sockaddr_len;
    struct sockaddr_storage addr;
} rb_addrinfo_t;

static void
addrinfo_mark(void *ptr)
{
    rb_addrinfo_t *rai = ptr;
    if (rai) {
        rb_gc_mark(rai->inspectname);
        rb_gc_mark(rai->canonname);
    }
}

#define addrinfo_free RUBY_TYPED_DEFAULT_FREE

static size_t
addrinfo_memsize(const void *ptr)
{
    return ptr ? sizeof(rb_addrinfo_t) : 0;
}

static const rb_data_type_t addrinfo_type = {
    "socket/addrinfo",
    {addrinfo_mark, addrinfo_free, addrinfo_memsize,},
};

static VALUE
addrinfo_s_allocate(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &addrinfo_type, 0);
}

#define IS_ADDRINFO(obj) rb_typeddata_is_kind_of((obj), &addrinfo_type)
static inline rb_addrinfo_t *
check_addrinfo(VALUE self)
{
    return rb_check_typeddata(self, &addrinfo_type);
}

static rb_addrinfo_t *
get_addrinfo(VALUE self)
{
    rb_addrinfo_t *rai = check_addrinfo(self);

    if (!rai) {
        rb_raise(rb_eTypeError, "uninitialized socket address");
    }
    return rai;
}


static rb_addrinfo_t *
alloc_addrinfo()
{
    rb_addrinfo_t *rai = ALLOC(rb_addrinfo_t);
    memset(rai, 0, sizeof(rb_addrinfo_t));
    rai->inspectname = Qnil;
    rai->canonname = Qnil;
    return rai;
}

static void
init_addrinfo(rb_addrinfo_t *rai, struct sockaddr *sa, socklen_t len,
              int pfamily, int socktype, int protocol,
              VALUE canonname, VALUE inspectname)
{
    if ((socklen_t)sizeof(rai->addr) < len)
        rb_raise(rb_eArgError, "sockaddr string too big");
    memcpy((void *)&rai->addr, (void *)sa, len);
    rai->sockaddr_len = len;

    rai->pfamily = pfamily;
    rai->socktype = socktype;
    rai->protocol = protocol;
    rai->canonname = canonname;
    rai->inspectname = inspectname;
}

VALUE
rsock_addrinfo_new(struct sockaddr *addr, socklen_t len,
                   int family, int socktype, int protocol,
                   VALUE canonname, VALUE inspectname)
{
    VALUE a;
    rb_addrinfo_t *rai;

    a = addrinfo_s_allocate(rb_cAddrinfo);
    DATA_PTR(a) = rai = alloc_addrinfo();
    init_addrinfo(rai, addr, len, family, socktype, protocol, canonname, inspectname);
    return a;
}

static struct addrinfo *
call_getaddrinfo(VALUE node, VALUE service,
                 VALUE family, VALUE socktype, VALUE protocol, VALUE flags,
                 int socktype_hack)
{
    struct addrinfo hints, *res;

    MEMZERO(&hints, struct addrinfo, 1);
    hints.ai_family = NIL_P(family) ? PF_UNSPEC : rsock_family_arg(family);

    if (!NIL_P(socktype)) {
	hints.ai_socktype = rsock_socktype_arg(socktype);
    }
    if (!NIL_P(protocol)) {
	hints.ai_protocol = NUM2INT(protocol);
    }
    if (!NIL_P(flags)) {
	hints.ai_flags = NUM2INT(flags);
    }
    res = rsock_getaddrinfo(node, service, &hints, socktype_hack);

    if (res == NULL)
	rb_raise(rb_eSocket, "host not found");
    return res;
}

static VALUE make_inspectname(VALUE node, VALUE service, struct addrinfo *res);

static void
init_addrinfo_getaddrinfo(rb_addrinfo_t *rai, VALUE node, VALUE service,
                          VALUE family, VALUE socktype, VALUE protocol, VALUE flags,
                          VALUE inspectnode, VALUE inspectservice)
{
    struct addrinfo *res = call_getaddrinfo(node, service, family, socktype, protocol, flags, 1);
    VALUE canonname;
    VALUE inspectname = rb_str_equal(node, inspectnode) ? Qnil : make_inspectname(inspectnode, inspectservice, res);

    canonname = Qnil;
    if (res->ai_canonname) {
        canonname = rb_tainted_str_new_cstr(res->ai_canonname);
        OBJ_FREEZE(canonname);
    }

    init_addrinfo(rai, res->ai_addr, res->ai_addrlen,
                  NUM2INT(family), NUM2INT(socktype), NUM2INT(protocol),
                  canonname, inspectname);

    freeaddrinfo(res);
}

static VALUE
make_inspectname(VALUE node, VALUE service, struct addrinfo *res)
{
    VALUE inspectname = Qnil;

    if (res) {
        /* drop redundant information which also shown in address:port part. */
        char hbuf[NI_MAXHOST], pbuf[NI_MAXSERV];
        int ret;
        ret = rb_getnameinfo(res->ai_addr, res->ai_addrlen, hbuf,
                             sizeof(hbuf), pbuf, sizeof(pbuf),
                             NI_NUMERICHOST|NI_NUMERICSERV);
        if (ret == 0) {
            if (TYPE(node) == T_STRING && strcmp(hbuf, RSTRING_PTR(node)) == 0)
                node = Qnil;
            if (TYPE(service) == T_STRING && strcmp(pbuf, RSTRING_PTR(service)) == 0)
                service = Qnil;
            else if (TYPE(service) == T_FIXNUM && atoi(pbuf) == FIX2INT(service))
                service = Qnil;
        }
    }

    if (TYPE(node) == T_STRING) {
        inspectname = rb_str_dup(node);
    }
    if (TYPE(service) == T_STRING) {
        if (NIL_P(inspectname))
            inspectname = rb_sprintf(":%s", StringValueCStr(service));
        else
            rb_str_catf(inspectname, ":%s", StringValueCStr(service));
    }
    else if (TYPE(service) == T_FIXNUM && FIX2INT(service) != 0)
    {
        if (NIL_P(inspectname))
            inspectname = rb_sprintf(":%d", FIX2INT(service));
        else
            rb_str_catf(inspectname, ":%d", FIX2INT(service));
    }
    if (!NIL_P(inspectname)) {
        OBJ_INFECT(inspectname, node);
        OBJ_INFECT(inspectname, service);
        OBJ_FREEZE(inspectname);
    }
    return inspectname;
}

static VALUE
addrinfo_firstonly_new(VALUE node, VALUE service, VALUE family, VALUE socktype, VALUE protocol, VALUE flags)
{
    VALUE ret;
    VALUE canonname;
    VALUE inspectname;

    struct addrinfo *res = call_getaddrinfo(node, service, family, socktype, protocol, flags, 0);

    inspectname = make_inspectname(node, service, res);

    canonname = Qnil;
    if (res->ai_canonname) {
        canonname = rb_tainted_str_new_cstr(res->ai_canonname);
        OBJ_FREEZE(canonname);
    }

    ret = rsock_addrinfo_new(res->ai_addr, res->ai_addrlen,
                             res->ai_family, res->ai_socktype, res->ai_protocol,
                             canonname, inspectname);

    freeaddrinfo(res);
    return ret;
}

static VALUE
addrinfo_list_new(VALUE node, VALUE service, VALUE family, VALUE socktype, VALUE protocol, VALUE flags)
{
    VALUE ret;
    struct addrinfo *r;
    VALUE inspectname;

    struct addrinfo *res = call_getaddrinfo(node, service, family, socktype, protocol, flags, 0);

    inspectname = make_inspectname(node, service, res);

    ret = rb_ary_new();
    for (r = res; r; r = r->ai_next) {
        VALUE addr;
        VALUE canonname = Qnil;

        if (r->ai_canonname) {
            canonname = rb_tainted_str_new_cstr(r->ai_canonname);
            OBJ_FREEZE(canonname);
        }

        addr = rsock_addrinfo_new(r->ai_addr, r->ai_addrlen,
                                  r->ai_family, r->ai_socktype, r->ai_protocol,
                                  canonname, inspectname);

        rb_ary_push(ret, addr);
    }

    freeaddrinfo(res);
    return ret;
}


#ifdef HAVE_SYS_UN_H
static void
init_unix_addrinfo(rb_addrinfo_t *rai, VALUE path, int socktype)
{
    struct sockaddr_un un;

    StringValue(path);

    if (sizeof(un.sun_path) <= (size_t)RSTRING_LEN(path))
        rb_raise(rb_eArgError, "too long unix socket path (max: %dbytes)",
            (int)sizeof(un.sun_path)-1);

    MEMZERO(&un, struct sockaddr_un, 1);

    un.sun_family = AF_UNIX;
    memcpy((void*)&un.sun_path, RSTRING_PTR(path), RSTRING_LEN(path));

    init_addrinfo(rai, (struct sockaddr *)&un, (socklen_t)sizeof(un),
		  PF_UNIX, socktype, 0, Qnil, Qnil);
}
#endif

/*
 * call-seq:
 *   Addrinfo.new(sockaddr)                             => addrinfo
 *   Addrinfo.new(sockaddr, family)                     => addrinfo
 *   Addrinfo.new(sockaddr, family, socktype)           => addrinfo
 *   Addrinfo.new(sockaddr, family, socktype, protocol) => addrinfo
 *
 * returns a new instance of Addrinfo.
 * The instance contains sockaddr, family, socktype, protocol.
 * sockaddr means struct sockaddr which can be used for connect(2), etc.
 * family, socktype and protocol are integers which is used for arguments of socket(2).
 *
 * sockaddr is specified as an array or a string.
 * The array should be compatible to the value of IPSocket#addr or UNIXSocket#addr.
 * The string should be struct sockaddr as generated by
 * Socket.sockaddr_in or Socket.unpack_sockaddr_un.
 *
 * sockaddr examples:
 * - ["AF_INET", 46102, "localhost.localdomain", "127.0.0.1"]
 * - ["AF_INET6", 42304, "ip6-localhost", "::1"]
 * - ["AF_UNIX", "/tmp/sock"]
 * - Socket.sockaddr_in("smtp", "2001:DB8::1")
 * - Socket.sockaddr_in(80, "172.18.22.42")
 * - Socket.sockaddr_in(80, "www.ruby-lang.org")
 * - Socket.sockaddr_un("/tmp/sock")
 *
 * In an AF_INET/AF_INET6 sockaddr array, the 4th element,
 * numeric IP address, is used to construct socket address in the Addrinfo instance.
 * If the 3rd element, textual host name, is non-nil, it is also recorded but used only for Addrinfo#inspect.
 *
 * family is specified as an integer to specify the protocol family such as Socket::PF_INET.
 * It can be a symbol or a string which is the constant name
 * with or without PF_ prefix such as :INET, :INET6, :UNIX, "PF_INET", etc.
 * If omitted, PF_UNSPEC is assumed.
 *
 * socktype is specified as an integer to specify the socket type such as Socket::SOCK_STREAM.
 * It can be a symbol or a string which is the constant name
 * with or without SOCK_ prefix such as :STREAM, :DGRAM, :RAW, "SOCK_STREAM", etc.
 * If omitted, 0 is assumed.
 *
 * protocol is specified as an integer to specify the protocol such as Socket::IPPROTO_TCP.
 * It must be an integer, unlike family and socktype.
 * If omitted, 0 is assumed.
 * Note that 0 is reasonable value for most protocols, except raw socket.
 *
 */
static VALUE
addrinfo_initialize(int argc, VALUE *argv, VALUE self)
{
    rb_addrinfo_t *rai;
    VALUE sockaddr_arg, sockaddr_ary, pfamily, socktype, protocol;
    int i_pfamily, i_socktype, i_protocol;
    struct sockaddr *sockaddr_ptr;
    socklen_t sockaddr_len;
    VALUE canonname = Qnil, inspectname = Qnil;

    if (check_addrinfo(self))
        rb_raise(rb_eTypeError, "already initialized socket address");
    DATA_PTR(self) = rai = alloc_addrinfo();

    rb_scan_args(argc, argv, "13", &sockaddr_arg, &pfamily, &socktype, &protocol);

    i_pfamily = NIL_P(pfamily) ? PF_UNSPEC : rsock_family_arg(pfamily);
    i_socktype = NIL_P(socktype) ? 0 : rsock_socktype_arg(socktype);
    i_protocol = NIL_P(protocol) ? 0 : NUM2INT(protocol);

    sockaddr_ary = rb_check_array_type(sockaddr_arg);
    if (!NIL_P(sockaddr_ary)) {
        VALUE afamily = rb_ary_entry(sockaddr_ary, 0);
        int af;
        StringValue(afamily);
        if (rsock_family_to_int(RSTRING_PTR(afamily), RSTRING_LEN(afamily), &af) == -1)
	    rb_raise(rb_eSocket, "unknown address family: %s", StringValueCStr(afamily));
        switch (af) {
          case AF_INET: /* ["AF_INET", 46102, "localhost.localdomain", "127.0.0.1"] */
#ifdef INET6
          case AF_INET6: /* ["AF_INET6", 42304, "ip6-localhost", "::1"] */
#endif
          {
            VALUE service = rb_ary_entry(sockaddr_ary, 1);
            VALUE nodename = rb_ary_entry(sockaddr_ary, 2);
            VALUE numericnode = rb_ary_entry(sockaddr_ary, 3);
            int flags;

            service = INT2NUM(NUM2INT(service));
            if (!NIL_P(nodename))
                StringValue(nodename);
            StringValue(numericnode);
            flags = AI_NUMERICHOST;
#ifdef AI_NUMERICSERV
            flags |= AI_NUMERICSERV;
#endif

            init_addrinfo_getaddrinfo(rai, numericnode, service,
                    INT2NUM(i_pfamily ? i_pfamily : af), INT2NUM(i_socktype), INT2NUM(i_protocol),
                    INT2NUM(flags),
                    nodename, service);
            break;
          }

#ifdef HAVE_SYS_UN_H
          case AF_UNIX: /* ["AF_UNIX", "/tmp/sock"] */
          {
            VALUE path = rb_ary_entry(sockaddr_ary, 1);
            StringValue(path);
            init_unix_addrinfo(rai, path, SOCK_STREAM);
            break;
          }
#endif

          default:
            rb_raise(rb_eSocket, "unexpected address family");
        }
    }
    else {
        StringValue(sockaddr_arg);
        sockaddr_ptr = (struct sockaddr *)RSTRING_PTR(sockaddr_arg);
        sockaddr_len = RSTRING_LENINT(sockaddr_arg);
        init_addrinfo(rai, sockaddr_ptr, sockaddr_len,
                      i_pfamily, i_socktype, i_protocol,
                      canonname, inspectname);
    }

    return self;
}

static int
get_afamily(struct sockaddr *addr, socklen_t len)
{
    if ((socklen_t)((char*)&addr->sa_family + sizeof(addr->sa_family) - (char*)addr) <= len)
        return addr->sa_family;
    else
        return AF_UNSPEC;
}

static int
ai_get_afamily(rb_addrinfo_t *rai)
{
    return get_afamily((struct sockaddr *)&rai->addr, rai->sockaddr_len);
}

static VALUE
inspect_sockaddr(VALUE addrinfo, VALUE ret)
{
    rb_addrinfo_t *rai = get_addrinfo(addrinfo);

    if (rai->sockaddr_len == 0) {
        rb_str_cat2(ret, "empty-sockaddr");
    }
    else if ((long)rai->sockaddr_len < ((char*)&rai->addr.ss_family + sizeof(rai->addr.ss_family)) - (char*)&rai->addr)
        rb_str_cat2(ret, "too-short-sockaddr");
    else {
        switch (rai->addr.ss_family) {
          case AF_INET:
          {
            struct sockaddr_in *addr;
            int port;
            if (rai->sockaddr_len < (socklen_t)sizeof(struct sockaddr_in)) {
                rb_str_cat2(ret, "too-short-AF_INET-sockaddr");
            }
            else {
                addr = (struct sockaddr_in *)&rai->addr;
                rb_str_catf(ret, "%d.%d.%d.%d",
                            ((unsigned char*)&addr->sin_addr)[0],
                            ((unsigned char*)&addr->sin_addr)[1],
                            ((unsigned char*)&addr->sin_addr)[2],
                            ((unsigned char*)&addr->sin_addr)[3]);
                port = ntohs(addr->sin_port);
                if (port)
                    rb_str_catf(ret, ":%d", port);
                if ((socklen_t)sizeof(struct sockaddr_in) < rai->sockaddr_len)
                    rb_str_catf(ret, "(sockaddr %d bytes too long)", (int)(rai->sockaddr_len - sizeof(struct sockaddr_in)));
            }
            break;
          }

#ifdef AF_INET6
          case AF_INET6:
          {
            struct sockaddr_in6 *addr;
            char hbuf[1024];
            int port;
            int error;
            if (rai->sockaddr_len < (socklen_t)sizeof(struct sockaddr_in6)) {
                rb_str_cat2(ret, "too-short-AF_INET6-sockaddr");
            }
            else {
                addr = (struct sockaddr_in6 *)&rai->addr;
                /* use getnameinfo for scope_id.
                 * RFC 4007: IPv6 Scoped Address Architecture
                 * draft-ietf-ipv6-scope-api-00.txt: Scoped Address Extensions to the IPv6 Basic Socket API
                 */
                error = getnameinfo((struct sockaddr *)&rai->addr, rai->sockaddr_len,
                                    hbuf, (socklen_t)sizeof(hbuf), NULL, 0,
                                    NI_NUMERICHOST|NI_NUMERICSERV);
                if (error) {
                    rsock_raise_socket_error("getnameinfo", error);
                }
                if (addr->sin6_port == 0) {
                    rb_str_cat2(ret, hbuf);
                }
                else {
                    port = ntohs(addr->sin6_port);
                    rb_str_catf(ret, "[%s]:%d", hbuf, port);
                }
                if ((socklen_t)sizeof(struct sockaddr_in6) < rai->sockaddr_len)
                    rb_str_catf(ret, "(sockaddr %d bytes too long)", (int)(rai->sockaddr_len - sizeof(struct sockaddr_in6)));
            }
            break;
          }
#endif

#ifdef HAVE_SYS_UN_H
          case AF_UNIX:
          {
            struct sockaddr_un *addr = (struct sockaddr_un *)&rai->addr;
            char *p, *s, *t, *e;
            s = addr->sun_path;
            e = (char*)addr + rai->sockaddr_len;
            if (e < s)
                rb_str_cat2(ret, "too-short-AF_UNIX-sockaddr");
            else if (s == e)
                rb_str_cat2(ret, "empty-path-AF_UNIX-sockaddr");
            else {
                int printable_only = 1;
                p = s;
                while (p < e && *p != '\0') {
                    printable_only = printable_only && ISPRINT(*p) && !ISSPACE(*p);
                    p++;
                }
                t = p;
                while (p < e && *p == '\0')
                    p++;
                if (printable_only && /* only printable, no space */
                    t < e && /* NUL terminated */
                    p == e) { /* no data after NUL */
		    if (s == t)
			rb_str_cat2(ret, "empty-path-AF_UNIX-sockaddr");
		    else if (s[0] == '/') /* absolute path */
			rb_str_cat2(ret, s);
		    else
                        rb_str_catf(ret, "AF_UNIX %s", s);
                }
                else {
                    rb_str_cat2(ret, "AF_UNIX");
                    e = (char *)addr->sun_path + sizeof(addr->sun_path);
                    while (s < e && *(e-1) == '\0')
                        e--;
                    while (s < e)
                        rb_str_catf(ret, ":%02x", (unsigned char)*s++);
                }
                if (addr->sun_path + sizeof(addr->sun_path) < (char*)&rai->addr + rai->sockaddr_len)
                    rb_str_catf(ret, "(sockaddr %d bytes too long)",
                            (int)(rai->sockaddr_len - (addr->sun_path + sizeof(addr->sun_path) - (char*)&rai->addr)));
            }
            break;
          }
#endif

          default:
          {
            ID id = rsock_intern_family(rai->addr.ss_family);
            if (id == 0)
                rb_str_catf(ret, "unknown address family %d", rai->addr.ss_family);
            else
                rb_str_catf(ret, "%s address format unknown", rb_id2name(id));
            break;
          }
        }
    }

    return ret;
}

/*
 * call-seq:
 *   addrinfo.inspect => string
 *
 * returns a string which shows addrinfo in human-readable form.
 *
 *   Addrinfo.tcp("localhost", 80).inspect #=> "#<Addrinfo: 127.0.0.1:80 TCP (localhost)>"
 *   Addrinfo.unix("/tmp/sock").inspect    #=> "#<Addrinfo: /tmp/sock SOCK_STREAM>"
 *
 */
static VALUE
addrinfo_inspect(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int internet_p;
    VALUE ret;

    ret = rb_sprintf("#<%s: ", rb_obj_classname(self));

    inspect_sockaddr(self, ret);

    if (rai->pfamily && ai_get_afamily(rai) != rai->pfamily) {
        ID id = rsock_intern_protocol_family(rai->pfamily);
        if (id)
            rb_str_catf(ret, " %s", rb_id2name(id));
        else
            rb_str_catf(ret, " PF_\?\?\?(%d)", rai->pfamily);
    }

    internet_p = rai->pfamily == PF_INET;
#ifdef INET6
    internet_p = internet_p || rai->pfamily == PF_INET6;
#endif
    if (internet_p && rai->socktype == SOCK_STREAM &&
        (rai->protocol == 0 || rai->protocol == IPPROTO_TCP)) {
        rb_str_cat2(ret, " TCP");
    }
    else if (internet_p && rai->socktype == SOCK_DGRAM &&
        (rai->protocol == 0 || rai->protocol == IPPROTO_UDP)) {
        rb_str_cat2(ret, " UDP");
    }
    else {
        if (rai->socktype) {
            ID id = rsock_intern_socktype(rai->socktype);
            if (id)
                rb_str_catf(ret, " %s", rb_id2name(id));
            else
                rb_str_catf(ret, " SOCK_\?\?\?(%d)", rai->socktype);
        }

        if (rai->protocol) {
            if (internet_p) {
                ID id = rsock_intern_ipproto(rai->protocol);
                if (id)
                    rb_str_catf(ret, " %s", rb_id2name(id));
                else
                    goto unknown_protocol;
            }
            else {
              unknown_protocol:
                rb_str_catf(ret, " UNKNOWN_PROTOCOL(%d)", rai->protocol);
            }
        }
    }

    if (!NIL_P(rai->canonname)) {
        VALUE name = rai->canonname;
        rb_str_catf(ret, " %s", StringValueCStr(name));
    }

    if (!NIL_P(rai->inspectname)) {
        VALUE name = rai->inspectname;
        rb_str_catf(ret, " (%s)", StringValueCStr(name));
    }

    rb_str_buf_cat2(ret, ">");
    return ret;
}

/*
 * call-seq:
 *   addrinfo.inspect_sockaddr => string
 *
 * returns a string which shows the sockaddr in _addrinfo_ with human-readable form.
 *
 *   Addrinfo.tcp("localhost", 80).inspect_sockaddr     #=> "127.0.0.1:80"
 *   Addrinfo.tcp("ip6-localhost", 80).inspect_sockaddr #=> "[::1]:80"
 *   Addrinfo.unix("/tmp/sock").inspect_sockaddr        #=> "/tmp/sock"
 *
 */
static VALUE
addrinfo_inspect_sockaddr(VALUE self)
{
    return inspect_sockaddr(self, rb_str_new("", 0));
}

/* :nodoc: */
static VALUE
addrinfo_mdump(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    VALUE sockaddr, afamily, pfamily, socktype, protocol, canonname, inspectname;
    int afamily_int = ai_get_afamily(rai);
    ID id;

    id = rsock_intern_protocol_family(rai->pfamily);
    if (id == 0)
        rb_raise(rb_eSocket, "unknown protocol family: %d", rai->pfamily);
    pfamily = rb_id2str(id);

    if (rai->socktype == 0)
        socktype = INT2FIX(0);
    else {
        id = rsock_intern_socktype(rai->socktype);
        if (id == 0)
            rb_raise(rb_eSocket, "unknown socktype: %d", rai->socktype);
        socktype = rb_id2str(id);
    }

    if (rai->protocol == 0)
        protocol = INT2FIX(0);
    else if (IS_IP_FAMILY(afamily_int)) {
        id = rsock_intern_ipproto(rai->protocol);
        if (id == 0)
            rb_raise(rb_eSocket, "unknown IP protocol: %d", rai->protocol);
        protocol = rb_id2str(id);
    }
    else {
        rb_raise(rb_eSocket, "unknown protocol: %d", rai->protocol);
    }

    canonname = rai->canonname;

    inspectname = rai->inspectname;

    id = rsock_intern_family(afamily_int);
    if (id == 0)
        rb_raise(rb_eSocket, "unknown address family: %d", afamily_int);
    afamily = rb_id2str(id);

    switch(afamily_int) {
#ifdef HAVE_SYS_UN_H
      case AF_UNIX:
      {
        struct sockaddr_un *su = (struct sockaddr_un *)&rai->addr;
        char *s, *e;
        s = su->sun_path;
        e = (char*)s + sizeof(su->sun_path);
        while (s < e && *(e-1) == '\0')
            e--;
        sockaddr = rb_str_new(s, e-s);
        break;
      }
#endif

      default:
      {
        char hbuf[NI_MAXHOST], pbuf[NI_MAXSERV];
        int error;
        error = getnameinfo((struct sockaddr *)&rai->addr, rai->sockaddr_len,
                            hbuf, (socklen_t)sizeof(hbuf), pbuf, (socklen_t)sizeof(pbuf),
                            NI_NUMERICHOST|NI_NUMERICSERV);
        if (error) {
            rsock_raise_socket_error("getnameinfo", error);
        }
        sockaddr = rb_assoc_new(rb_str_new_cstr(hbuf), rb_str_new_cstr(pbuf));
        break;
      }
    }

    return rb_ary_new3(7, afamily, sockaddr, pfamily, socktype, protocol, canonname, inspectname);
}

/* :nodoc: */
static VALUE
addrinfo_mload(VALUE self, VALUE ary)
{
    VALUE v;
    VALUE canonname, inspectname;
    int afamily, pfamily, socktype, protocol;
    struct sockaddr_storage ss;
    socklen_t len;
    rb_addrinfo_t *rai;

    if (check_addrinfo(self))
        rb_raise(rb_eTypeError, "already initialized socket address");

    ary = rb_convert_type(ary, T_ARRAY, "Array", "to_ary");

    v = rb_ary_entry(ary, 0);
    StringValue(v);
    if (rsock_family_to_int(RSTRING_PTR(v), RSTRING_LEN(v), &afamily) == -1)
        rb_raise(rb_eTypeError, "unexpected address family");

    v = rb_ary_entry(ary, 2);
    StringValue(v);
    if (rsock_family_to_int(RSTRING_PTR(v), RSTRING_LEN(v), &pfamily) == -1)
        rb_raise(rb_eTypeError, "unexpected protocol family");

    v = rb_ary_entry(ary, 3);
    if (v == INT2FIX(0))
        socktype = 0;
    else {
        StringValue(v);
        if (rsock_socktype_to_int(RSTRING_PTR(v), RSTRING_LEN(v), &socktype) == -1)
            rb_raise(rb_eTypeError, "unexpected socktype");
    }

    v = rb_ary_entry(ary, 4);
    if (v == INT2FIX(0))
        protocol = 0;
    else {
        StringValue(v);
        if (IS_IP_FAMILY(afamily)) {
            if (rsock_ipproto_to_int(RSTRING_PTR(v), RSTRING_LEN(v), &protocol) == -1)
                rb_raise(rb_eTypeError, "unexpected protocol");
        }
        else {
            rb_raise(rb_eTypeError, "unexpected protocol");
        }
    }

    v = rb_ary_entry(ary, 5);
    if (NIL_P(v))
        canonname = Qnil;
    else {
        StringValue(v);
        canonname = v;
    }

    v = rb_ary_entry(ary, 6);
    if (NIL_P(v))
        inspectname = Qnil;
    else {
        StringValue(v);
        inspectname = v;
    }

    v = rb_ary_entry(ary, 1);
    switch(afamily) {
#ifdef HAVE_SYS_UN_H
      case AF_UNIX:
      {
        struct sockaddr_un uaddr;
        memset(&uaddr, 0, sizeof(uaddr));
        uaddr.sun_family = AF_UNIX;

        StringValue(v);
        if (sizeof(uaddr.sun_path) <= (size_t)RSTRING_LEN(v))
            rb_raise(rb_eSocket, "too long AF_UNIX path");
        memcpy(uaddr.sun_path, RSTRING_PTR(v), RSTRING_LEN(v));
        len = (socklen_t)sizeof(uaddr);
        memcpy(&ss, &uaddr, len);
        break;
      }
#endif

      default:
      {
        VALUE pair = rb_convert_type(v, T_ARRAY, "Array", "to_ary");
        struct addrinfo *res;
        int flags = AI_NUMERICHOST;
#ifdef AI_NUMERICSERV
        flags |= AI_NUMERICSERV;
#endif
        res = call_getaddrinfo(rb_ary_entry(pair, 0), rb_ary_entry(pair, 1),
                               INT2NUM(pfamily), INT2NUM(socktype), INT2NUM(protocol),
                               INT2NUM(flags), 1);

        len = res->ai_addrlen;
        memcpy(&ss, res->ai_addr, res->ai_addrlen);
        break;
      }
    }

    DATA_PTR(self) = rai = alloc_addrinfo();
    init_addrinfo(rai, (struct sockaddr *)&ss, len,
                  pfamily, socktype, protocol,
                  canonname, inspectname);
    return self;
}

/*
 * call-seq:
 *   addrinfo.afamily => integer
 *
 * returns the address family as an integer.
 *
 *   Addrinfo.tcp("localhost", 80).afamily == Socket::AF_INET #=> true
 *
 */
static VALUE
addrinfo_afamily(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    return INT2NUM(ai_get_afamily(rai));
}

/*
 * call-seq:
 *   addrinfo.pfamily => integer
 *
 * returns the protocol family as an integer.
 *
 *   Addrinfo.tcp("localhost", 80).pfamily == Socket::PF_INET #=> true
 *
 */
static VALUE
addrinfo_pfamily(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    return INT2NUM(rai->pfamily);
}

/*
 * call-seq:
 *   addrinfo.socktype => integer
 *
 * returns the socket type as an integer.
 *
 *   Addrinfo.tcp("localhost", 80).socktype == Socket::SOCK_STREAM #=> true
 *
 */
static VALUE
addrinfo_socktype(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    return INT2NUM(rai->socktype);
}

/*
 * call-seq:
 *   addrinfo.protocol => integer
 *
 * returns the socket type as an integer.
 *
 *   Addrinfo.tcp("localhost", 80).protocol == Socket::IPPROTO_TCP #=> true
 *
 */
static VALUE
addrinfo_protocol(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    return INT2NUM(rai->protocol);
}

/*
 * call-seq:
 *   addrinfo.to_sockaddr => string
 *   addrinfo.to_s => string
 *
 * returns the socket address as packed struct sockaddr string.
 *
 *   Addrinfo.tcp("localhost", 80).to_sockaddr
 *   #=> "\x02\x00\x00P\x7F\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00"
 *
 */
static VALUE
addrinfo_to_sockaddr(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    VALUE ret;
    ret = rb_str_new((char*)&rai->addr, rai->sockaddr_len);
    OBJ_INFECT(ret, self);
    return ret;
}

/*
 * call-seq:
 *   addrinfo.canonname => string or nil
 *
 * returns the canonical name as an string.
 *
 * nil is returned if no canonical name.
 *
 * The canonical name is set by Addrinfo.getaddrinfo when AI_CANONNAME is specified.
 *
 *   list = Addrinfo.getaddrinfo("www.ruby-lang.org", 80, :INET, :STREAM, nil, Socket::AI_CANONNAME)
 *   p list[0] #=> #<Addrinfo: 221.186.184.68:80 TCP carbon.ruby-lang.org (www.ruby-lang.org)>
 *   p list[0].canonname #=> "carbon.ruby-lang.org"
 *
 */
static VALUE
addrinfo_canonname(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    return rai->canonname;
}

/*
 * call-seq:
 *   addrinfo.ip? => true or false
 *
 * returns true if addrinfo is internet (IPv4/IPv6) address.
 * returns false otherwise.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).ip? #=> true
 *   Addrinfo.tcp("::1", 80).ip?       #=> true
 *   Addrinfo.unix("/tmp/sock").ip?    #=> false
 *
 */
static VALUE
addrinfo_ip_p(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    return IS_IP_FAMILY(family) ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   addrinfo.ipv4? => true or false
 *
 * returns true if addrinfo is IPv4 address.
 * returns false otherwise.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).ipv4? #=> true
 *   Addrinfo.tcp("::1", 80).ipv4?       #=> false
 *   Addrinfo.unix("/tmp/sock").ipv4?    #=> false
 *
 */
static VALUE
addrinfo_ipv4_p(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    return ai_get_afamily(rai) == AF_INET ? Qtrue : Qfalse;
}

/*
 * call-seq:
 *   addrinfo.ipv6? => true or false
 *
 * returns true if addrinfo is IPv6 address.
 * returns false otherwise.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).ipv6? #=> false
 *   Addrinfo.tcp("::1", 80).ipv6?       #=> true
 *   Addrinfo.unix("/tmp/sock").ipv6?    #=> false
 *
 */
static VALUE
addrinfo_ipv6_p(VALUE self)
{
#ifdef AF_INET6
    rb_addrinfo_t *rai = get_addrinfo(self);
    return ai_get_afamily(rai) == AF_INET6 ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

/*
 * call-seq:
 *   addrinfo.unix? => true or false
 *
 * returns true if addrinfo is UNIX address.
 * returns false otherwise.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).unix? #=> false
 *   Addrinfo.tcp("::1", 80).unix?       #=> false
 *   Addrinfo.unix("/tmp/sock").unix?    #=> true
 *
 */
static VALUE
addrinfo_unix_p(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
#ifdef AF_UNIX
    return ai_get_afamily(rai) == AF_UNIX ? Qtrue : Qfalse;
#else
    return Qfalse;
#endif
}

/*
 * call-seq:
 *   addrinfo.getnameinfo        => [nodename, service]
 *   addrinfo.getnameinfo(flags) => [nodename, service]
 *
 * returns nodename and service as a pair of strings.
 * This converts struct sockaddr in addrinfo to textual representation.
 *
 * flags should be bitwise OR of Socket::NI_??? constants.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).getnameinfo #=> ["localhost", "www"]
 *
 *   Addrinfo.tcp("127.0.0.1", 80).getnameinfo(Socket::NI_NUMERICSERV)
 *   #=> ["localhost", "80"]
 */
static VALUE
addrinfo_getnameinfo(int argc, VALUE *argv, VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    VALUE vflags;
    char hbuf[1024], pbuf[1024];
    int flags, error;

    rb_scan_args(argc, argv, "01", &vflags);

    flags = NIL_P(vflags) ? 0 : NUM2INT(vflags);

    if (rai->socktype == SOCK_DGRAM)
        flags |= NI_DGRAM;

    error = getnameinfo((struct sockaddr *)&rai->addr, rai->sockaddr_len,
                        hbuf, (socklen_t)sizeof(hbuf), pbuf, (socklen_t)sizeof(pbuf),
                        flags);
    if (error) {
        rsock_raise_socket_error("getnameinfo", error);
    }

    return rb_assoc_new(rb_str_new2(hbuf), rb_str_new2(pbuf));
}

/*
 * call-seq:
 *   addrinfo.ip_unpack => [addr, port]
 *
 * Returns the IP address and port number as 2-element array.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).ip_unpack    #=> ["127.0.0.1", 80]
 *   Addrinfo.tcp("::1", 80).ip_unpack          #=> ["::1", 80]
 */
static VALUE
addrinfo_ip_unpack(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    VALUE vflags;
    VALUE ret, portstr;

    if (!IS_IP_FAMILY(family))
	rb_raise(rb_eSocket, "need IPv4 or IPv6 address");

    vflags = INT2NUM(NI_NUMERICHOST|NI_NUMERICSERV);
    ret = addrinfo_getnameinfo(1, &vflags, self);
    portstr = rb_ary_entry(ret, 1);
    rb_ary_store(ret, 1, INT2NUM(atoi(StringValueCStr(portstr))));
    return ret;
}

/*
 * call-seq:
 *   addrinfo.ip_address => string
 *
 * Returns the IP address as a string.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).ip_address    #=> "127.0.0.1"
 *   Addrinfo.tcp("::1", 80).ip_address          #=> "::1"
 */
static VALUE
addrinfo_ip_address(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    VALUE vflags;
    VALUE ret;

    if (!IS_IP_FAMILY(family))
	rb_raise(rb_eSocket, "need IPv4 or IPv6 address");

    vflags = INT2NUM(NI_NUMERICHOST|NI_NUMERICSERV);
    ret = addrinfo_getnameinfo(1, &vflags, self);
    return rb_ary_entry(ret, 0);
}

/*
 * call-seq:
 *   addrinfo.ip_port => port
 *
 * Returns the port number as an integer.
 *
 *   Addrinfo.tcp("127.0.0.1", 80).ip_port    #=> 80
 *   Addrinfo.tcp("::1", 80).ip_port          #=> 80
 */
static VALUE
addrinfo_ip_port(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    int port;

    if (!IS_IP_FAMILY(family)) {
      bad_family:
#ifdef AF_INET6
	rb_raise(rb_eSocket, "need IPv4 or IPv6 address");
#else
	rb_raise(rb_eSocket, "need IPv4 address");
#endif
    }

    switch (family) {
      case AF_INET:
        if (rai->sockaddr_len != sizeof(struct sockaddr_in))
            rb_raise(rb_eSocket, "unexpected sockaddr size for IPv4");
        port = ntohs(((struct sockaddr_in *)&rai->addr)->sin_port);
        break;

#ifdef AF_INET6
      case AF_INET6:
        if (rai->sockaddr_len != sizeof(struct sockaddr_in6))
            rb_raise(rb_eSocket, "unexpected sockaddr size for IPv6");
        port = ntohs(((struct sockaddr_in6 *)&rai->addr)->sin6_port);
        break;
#endif

      default:
	goto bad_family;
    }

    return INT2NUM(port);
}

static int
extract_in_addr(VALUE self, uint32_t *addrp)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    if (family != AF_INET) return 0;
    *addrp = ntohl(((struct sockaddr_in *)&rai->addr)->sin_addr.s_addr);
    return 1;
}

/*
 * Returns true for IPv4 private address (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv4_private_p(VALUE self)
{
    uint32_t a;
    if (!extract_in_addr(self, &a)) return Qfalse;
    if ((a & 0xff000000) == 0x0a000000 || /* 10.0.0.0/8 */
        (a & 0xfff00000) == 0xac100000 || /* 172.16.0.0/12 */
        (a & 0xffff0000) == 0xc0a80000)   /* 192.168.0.0/16 */
        return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv4 loopback address (127.0.0.0/8).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv4_loopback_p(VALUE self)
{
    uint32_t a;
    if (!extract_in_addr(self, &a)) return Qfalse;
    if ((a & 0xff000000) == 0x7f000000) /* 127.0.0.0/8 */
        return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv4 multicast address (224.0.0.0/4).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv4_multicast_p(VALUE self)
{
    uint32_t a;
    if (!extract_in_addr(self, &a)) return Qfalse;
    if ((a & 0xf0000000) == 0xe0000000) /* 224.0.0.0/4 */
        return Qtrue;
    return Qfalse;
}

#ifdef INET6

static struct in6_addr *
extract_in6_addr(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    if (family != AF_INET6) return NULL;
    return &((struct sockaddr_in6 *)&rai->addr)->sin6_addr;
}

/*
 * Returns true for IPv6 unspecified address (::).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_unspecified_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_UNSPECIFIED(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 loopback address (::1).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_loopback_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_LOOPBACK(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 multicast address (ff00::/8).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_multicast_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_MULTICAST(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 link local address (ff80::/10).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_linklocal_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_LINKLOCAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 site local address (ffc0::/10).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_sitelocal_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_SITELOCAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv4-mapped IPv6 address (::ffff:0:0/80).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_v4mapped_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_V4MAPPED(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv4-compatible IPv6 address (::/80).
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_v4compat_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_V4COMPAT(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 multicast node-local scope address.
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_mc_nodelocal_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_MC_NODELOCAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 multicast link-local scope address.
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_mc_linklocal_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_MC_LINKLOCAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 multicast site-local scope address.
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_mc_sitelocal_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_MC_SITELOCAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 multicast organization-local scope address.
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_mc_orglocal_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_MC_ORGLOCAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns true for IPv6 multicast global scope address.
 * It returns false otherwise.
 */
static VALUE
addrinfo_ipv6_mc_global_p(VALUE self)
{
    struct in6_addr *addr = extract_in6_addr(self);
    if (addr && IN6_IS_ADDR_MC_GLOBAL(addr)) return Qtrue;
    return Qfalse;
}

/*
 * Returns IPv4 address of IPv4 mapped/compatible IPv6 address.
 * It returns nil if +self+ is not IPv4 mapped/compatible IPv6 address.
 *
 *   Addrinfo.ip("::192.0.2.3").ipv6_to_ipv4      #=> #<Addrinfo: 192.0.2.3>
 *   Addrinfo.ip("::ffff:192.0.2.3").ipv6_to_ipv4 #=> #<Addrinfo: 192.0.2.3>
 *   Addrinfo.ip("::1").ipv6_to_ipv4              #=> nil
 *   Addrinfo.ip("192.0.2.3").ipv6_to_ipv4        #=> nil
 *   Addrinfo.unix("/tmp/sock").ipv6_to_ipv4      #=> nil
 */
static VALUE
addrinfo_ipv6_to_ipv4(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    struct in6_addr *addr;
    int family = ai_get_afamily(rai);
    if (family != AF_INET6) return Qnil;
    addr = &((struct sockaddr_in6 *)&rai->addr)->sin6_addr;
    if (IN6_IS_ADDR_V4MAPPED(addr) || IN6_IS_ADDR_V4COMPAT(addr)) {
        struct sockaddr_in sin4;
        MEMZERO(&sin4, struct sockaddr_in, 1);
        sin4.sin_family = AF_INET;
        SET_SIN_LEN(&sin4, sizeof(sin4));
        memcpy(&sin4.sin_addr, (char*)addr + sizeof(*addr) - sizeof(sin4.sin_addr), sizeof(sin4.sin_addr));
        return rsock_addrinfo_new((struct sockaddr *)&sin4, (socklen_t)sizeof(sin4),
                                  PF_INET, rai->socktype, rai->protocol,
                                  rai->canonname, rai->inspectname);
    }
    else {
        return Qnil;
    }
}

#endif

#ifdef HAVE_SYS_UN_H
/*
 * call-seq:
 *   addrinfo.unix_path => path
 *
 * Returns the socket path as a string.
 *
 *   Addrinfo.unix("/tmp/sock").unix_path       #=> "/tmp/sock"
 */
static VALUE
addrinfo_unix_path(VALUE self)
{
    rb_addrinfo_t *rai = get_addrinfo(self);
    int family = ai_get_afamily(rai);
    struct sockaddr_un *addr;
    char *s, *e;

    if (family != AF_UNIX)
	rb_raise(rb_eSocket, "need AF_UNIX address");

    addr = (struct sockaddr_un *)&rai->addr;

    s = addr->sun_path;
    e = (char*)addr + rai->sockaddr_len;
    if (e < s)
        rb_raise(rb_eSocket, "too short AF_UNIX address");
    if (addr->sun_path + sizeof(addr->sun_path) < e)
        rb_raise(rb_eSocket, "too long AF_UNIX address");
    while (s < e && *(e-1) == '\0')
        e--;
    return rb_str_new(s, e-s);
}
#endif

/*
 * call-seq:
 *   Addrinfo.getaddrinfo(nodename, service, family, socktype, protocol, flags) => [addrinfo, ...]
 *   Addrinfo.getaddrinfo(nodename, service, family, socktype, protocol)        => [addrinfo, ...]
 *   Addrinfo.getaddrinfo(nodename, service, family, socktype)                  => [addrinfo, ...]
 *   Addrinfo.getaddrinfo(nodename, service, family)                            => [addrinfo, ...]
 *   Addrinfo.getaddrinfo(nodename, service)                                    => [addrinfo, ...]
 *
 * returns a list of addrinfo objects as an array.
 *
 * This method converts nodename (hostname) and service (port) to addrinfo.
 * Since the conversion is not unique, the result is a list of addrinfo objects.
 *
 * nodename or service can be nil if no conversion intended.
 *
 * family, socktype and protocol are hint for preferred protocol.
 * If the result will be used for a socket with SOCK_STREAM,
 * SOCK_STREAM should be specified as socktype.
 * If so, Addrinfo.getaddrinfo returns addrinfo list appropriate for SOCK_STREAM.
 * If they are omitted or nil is given, the result is not restricted.
 *
 * Similarly, PF_INET6 as family restricts for IPv6.
 *
 * flags should be bitwise OR of Socket::AI_??? constants.
 *
 * Note that socktype should be specified whenever application knows the usage of the address.
 * Some platform causes an error when socktype is omitted and servname is specified as an integer
 * because some port numbers, 512 for example, are ambiguous without socktype.
 *
 *   Addrinfo.getaddrinfo("www.kame.net", 80, nil, :STREAM)
 *   #=> [#<Addrinfo: 203.178.141.194:80 TCP (www.kame.net)>,
 *   #    #<Addrinfo: [2001:200:dff:fff1:216:3eff:feb1:44d7]:80 TCP (www.kame.net)>]
 *
 */
static VALUE
addrinfo_s_getaddrinfo(int argc, VALUE *argv, VALUE self)
{
    VALUE node, service, family, socktype, protocol, flags;

    rb_scan_args(argc, argv, "24", &node, &service, &family, &socktype, &protocol, &flags);
    return addrinfo_list_new(node, service, family, socktype, protocol, flags);
}

/*
 * call-seq:
 *   Addrinfo.ip(host) => addrinfo
 *
 * returns an addrinfo object for IP address.
 *
 * The port, socktype, protocol of the result is filled by zero.
 * So, it is not appropriate to create a socket.
 *
 *   Addrinfo.ip("localhost") #=> #<Addrinfo: 127.0.0.1 (localhost)>
 */
static VALUE
addrinfo_s_ip(VALUE self, VALUE host)
{
    VALUE ret;
    rb_addrinfo_t *rai;
    ret = addrinfo_firstonly_new(host, Qnil,
            INT2NUM(PF_UNSPEC), INT2FIX(0), INT2FIX(0), INT2FIX(0));
    rai = get_addrinfo(ret);
    rai->socktype = 0;
    rai->protocol = 0;
    return ret;
}

/*
 * call-seq:
 *   Addrinfo.tcp(host, port) => addrinfo
 *
 * returns an addrinfo object for TCP address.
 *
 *   Addrinfo.tcp("localhost", "smtp") #=> #<Addrinfo: 127.0.0.1:25 TCP (localhost:smtp)>
 */
static VALUE
addrinfo_s_tcp(VALUE self, VALUE host, VALUE port)
{
    return addrinfo_firstonly_new(host, port,
            INT2NUM(PF_UNSPEC), INT2NUM(SOCK_STREAM), INT2NUM(IPPROTO_TCP), INT2FIX(0));
}

/*
 * call-seq:
 *   Addrinfo.udp(host, port) => addrinfo
 *
 * returns an addrinfo object for UDP address.
 *
 *   Addrinfo.udp("localhost", "daytime") #=> #<Addrinfo: 127.0.0.1:13 UDP (localhost:daytime)>
 */
static VALUE
addrinfo_s_udp(VALUE self, VALUE host, VALUE port)
{
    return addrinfo_firstonly_new(host, port,
            INT2NUM(PF_UNSPEC), INT2NUM(SOCK_DGRAM), INT2NUM(IPPROTO_UDP), INT2FIX(0));
}

#ifdef HAVE_SYS_UN_H

/*
 * call-seq:
 *   Addrinfo.unix(path [, socktype]) => addrinfo
 *
 * returns an addrinfo object for UNIX socket address.
 *
 * _socktype_ specifies the socket type.
 * If it is omitted, :STREAM is used.
 *
 *   Addrinfo.unix("/tmp/sock")         #=> #<Addrinfo: /tmp/sock SOCK_STREAM>
 *   Addrinfo.unix("/tmp/sock", :DGRAM) #=> #<Addrinfo: /tmp/sock SOCK_DGRAM>
 */
static VALUE
addrinfo_s_unix(int argc, VALUE *argv, VALUE self)
{
    VALUE path, vsocktype, addr;
    int socktype;
    rb_addrinfo_t *rai;

    rb_scan_args(argc, argv, "11", &path, &vsocktype);

    if (NIL_P(vsocktype))
        socktype = SOCK_STREAM;
    else
        socktype = rsock_socktype_arg(vsocktype);

    addr = addrinfo_s_allocate(rb_cAddrinfo);
    DATA_PTR(addr) = rai = alloc_addrinfo();
    init_unix_addrinfo(rai, path, socktype);
    OBJ_INFECT(addr, path);
    return addr;
}

#endif

VALUE
rsock_sockaddr_string_value(volatile VALUE *v)
{
    VALUE val = *v;
    if (IS_ADDRINFO(val)) {
        *v = addrinfo_to_sockaddr(val);
    }
    StringValue(*v);
    return *v;
}

char *
rsock_sockaddr_string_value_ptr(volatile VALUE *v)
{
    rsock_sockaddr_string_value(v);
    return RSTRING_PTR(*v);
}

VALUE
rb_check_sockaddr_string_type(VALUE val)
{
    if (IS_ADDRINFO(val))
        return addrinfo_to_sockaddr(val);
    return rb_check_string_type(val);
}

VALUE
rsock_fd_socket_addrinfo(int fd, struct sockaddr *addr, socklen_t len)
{
    int family;
    int socktype;
    int ret;
    socklen_t optlen = (socklen_t)sizeof(socktype);

    /* assumes protocol family and address family are identical */
    family = get_afamily(addr, len);

    ret = getsockopt(fd, SOL_SOCKET, SO_TYPE, (void*)&socktype, &optlen);
    if (ret == -1) {
        rb_sys_fail("getsockopt(SO_TYPE)");
    }

    return rsock_addrinfo_new(addr, len, family, socktype, 0, Qnil, Qnil);
}

VALUE
rsock_io_socket_addrinfo(VALUE io, struct sockaddr *addr, socklen_t len)
{
    rb_io_t *fptr;

    switch (TYPE(io)) {
      case T_FIXNUM:
        return rsock_fd_socket_addrinfo(FIX2INT(io), addr, len);

      case T_BIGNUM:
        return rsock_fd_socket_addrinfo(NUM2INT(io), addr, len);

      case T_FILE:
        GetOpenFile(io, fptr);
        return rsock_fd_socket_addrinfo(fptr->fd, addr, len);

      default:
        rb_raise(rb_eTypeError, "neither IO nor file descriptor");
    }
}

/*
 * Addrinfo class
 */
void
rsock_init_addrinfo(void)
{
    /*
     * The Addrinfo class maps <tt>struct addrinfo</tt> to ruby.  This
     * structure identifies an Internet host and a service.
     */
    rb_cAddrinfo = rb_define_class("Addrinfo", rb_cData);
    rb_define_alloc_func(rb_cAddrinfo, addrinfo_s_allocate);
    rb_define_method(rb_cAddrinfo, "initialize", addrinfo_initialize, -1);
    rb_define_method(rb_cAddrinfo, "inspect", addrinfo_inspect, 0);
    rb_define_method(rb_cAddrinfo, "inspect_sockaddr", addrinfo_inspect_sockaddr, 0);
    rb_define_singleton_method(rb_cAddrinfo, "getaddrinfo", addrinfo_s_getaddrinfo, -1);
    rb_define_singleton_method(rb_cAddrinfo, "ip", addrinfo_s_ip, 1);
    rb_define_singleton_method(rb_cAddrinfo, "tcp", addrinfo_s_tcp, 2);
    rb_define_singleton_method(rb_cAddrinfo, "udp", addrinfo_s_udp, 2);
#ifdef HAVE_SYS_UN_H
    rb_define_singleton_method(rb_cAddrinfo, "unix", addrinfo_s_unix, -1);
#endif

    rb_define_method(rb_cAddrinfo, "afamily", addrinfo_afamily, 0);
    rb_define_method(rb_cAddrinfo, "pfamily", addrinfo_pfamily, 0);
    rb_define_method(rb_cAddrinfo, "socktype", addrinfo_socktype, 0);
    rb_define_method(rb_cAddrinfo, "protocol", addrinfo_protocol, 0);
    rb_define_method(rb_cAddrinfo, "canonname", addrinfo_canonname, 0);

    rb_define_method(rb_cAddrinfo, "ipv4?", addrinfo_ipv4_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6?", addrinfo_ipv6_p, 0);
    rb_define_method(rb_cAddrinfo, "unix?", addrinfo_unix_p, 0);

    rb_define_method(rb_cAddrinfo, "ip?", addrinfo_ip_p, 0);
    rb_define_method(rb_cAddrinfo, "ip_unpack", addrinfo_ip_unpack, 0);
    rb_define_method(rb_cAddrinfo, "ip_address", addrinfo_ip_address, 0);
    rb_define_method(rb_cAddrinfo, "ip_port", addrinfo_ip_port, 0);

    rb_define_method(rb_cAddrinfo, "ipv4_private?", addrinfo_ipv4_private_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv4_loopback?", addrinfo_ipv4_loopback_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv4_multicast?", addrinfo_ipv4_multicast_p, 0);

#ifdef INET6
    rb_define_method(rb_cAddrinfo, "ipv6_unspecified?", addrinfo_ipv6_unspecified_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_loopback?", addrinfo_ipv6_loopback_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_multicast?", addrinfo_ipv6_multicast_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_linklocal?", addrinfo_ipv6_linklocal_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_sitelocal?", addrinfo_ipv6_sitelocal_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_v4mapped?", addrinfo_ipv6_v4mapped_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_v4compat?", addrinfo_ipv6_v4compat_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_mc_nodelocal?", addrinfo_ipv6_mc_nodelocal_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_mc_linklocal?", addrinfo_ipv6_mc_linklocal_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_mc_sitelocal?", addrinfo_ipv6_mc_sitelocal_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_mc_orglocal?", addrinfo_ipv6_mc_orglocal_p, 0);
    rb_define_method(rb_cAddrinfo, "ipv6_mc_global?", addrinfo_ipv6_mc_global_p, 0);

    rb_define_method(rb_cAddrinfo, "ipv6_to_ipv4", addrinfo_ipv6_to_ipv4, 0);
#endif

#ifdef HAVE_SYS_UN_H
    rb_define_method(rb_cAddrinfo, "unix_path", addrinfo_unix_path, 0);
#endif

    rb_define_method(rb_cAddrinfo, "to_sockaddr", addrinfo_to_sockaddr, 0);
    rb_define_method(rb_cAddrinfo, "to_s", addrinfo_to_sockaddr, 0); /* compatibility for ruby before 1.9.2 */

    rb_define_method(rb_cAddrinfo, "getnameinfo", addrinfo_getnameinfo, -1);

    rb_define_method(rb_cAddrinfo, "marshal_dump", addrinfo_mdump, 0);
    rb_define_method(rb_cAddrinfo, "marshal_load", addrinfo_mload, 1);
}
