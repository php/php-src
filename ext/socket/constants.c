/************************************************

  constants.c -

  created at: Thu Mar 31 12:21:29 JST 1994

  Copyright (C) 1993-2007 Yukihiro Matsumoto

************************************************/

#include "rubysocket.h"

static VALUE rb_mSockConst;

#include "constdefs.c"

static int
constant_arg(VALUE arg, int (*str_to_int)(const char*, long, int*), const char *errmsg)
{
    VALUE tmp;
    char *ptr;
    int ret;

    if (SYMBOL_P(arg)) {
        arg = rb_sym_to_s(arg);
        goto str;
    }
    else if (!NIL_P(tmp = rb_check_string_type(arg))) {
	arg = tmp;
      str:
	rb_check_safe_obj(arg);
        ptr = RSTRING_PTR(arg);
        if (str_to_int(ptr, RSTRING_LEN(arg), &ret) == -1)
	    rb_raise(rb_eSocket, "%s: %s", errmsg, ptr);
    }
    else {
	ret = NUM2INT(arg);
    }
    return ret;
}

int
rsock_family_arg(VALUE domain)
{
    /* convert AF_INET, etc. */
    return constant_arg(domain, rsock_family_to_int, "unknown socket domain");
}

int
rsock_socktype_arg(VALUE type)
{
    /* convert SOCK_STREAM, etc. */
    return constant_arg(type, rsock_socktype_to_int, "unknown socket type");
}

int
rsock_level_arg(int family, VALUE level)
{
    /* convert SOL_SOCKET, IPPROTO_TCP, etc. */
    if (IS_IP_FAMILY(family)) {
        return constant_arg(level, rsock_ip_level_to_int, "unknown protocol level");
    }
    else {
        return constant_arg(level, rsock_unknown_level_to_int, "unknown protocol level");
    }
}

int
rsock_optname_arg(int family, int level, VALUE optname)
{
    if (IS_IP_FAMILY(family)) {
        switch (level) {
          case SOL_SOCKET:
            return constant_arg(optname, rsock_so_optname_to_int, "unknown socket level option name");
          case IPPROTO_IP:
            return constant_arg(optname, rsock_ip_optname_to_int, "unknown IP level option name");
#ifdef IPPROTO_IPV6
          case IPPROTO_IPV6:
            return constant_arg(optname, rsock_ipv6_optname_to_int, "unknown IPv6 level option name");
#endif
          case IPPROTO_TCP:
            return constant_arg(optname, rsock_tcp_optname_to_int, "unknown TCP level option name");
          case IPPROTO_UDP:
            return constant_arg(optname, rsock_udp_optname_to_int, "unknown UDP level option name");
          default:
            return NUM2INT(optname);
        }
    }
    else {
        switch (level) {
          case SOL_SOCKET:
            return constant_arg(optname, rsock_so_optname_to_int, "unknown socket level option name");
          default:
            return NUM2INT(optname);
        }
    }
}

int
rsock_cmsg_type_arg(int family, int level, VALUE type)
{
    if (IS_IP_FAMILY(family)) {
        switch (level) {
          case SOL_SOCKET:
            return constant_arg(type, rsock_scm_optname_to_int, "unknown UNIX control message");
          case IPPROTO_IP:
            return constant_arg(type, rsock_ip_optname_to_int, "unknown IP control message");
#ifdef INET6
          case IPPROTO_IPV6:
            return constant_arg(type, rsock_ipv6_optname_to_int, "unknown IPv6 control message");
#endif
          case IPPROTO_TCP:
            return constant_arg(type, rsock_tcp_optname_to_int, "unknown TCP control message");
          case IPPROTO_UDP:
            return constant_arg(type, rsock_udp_optname_to_int, "unknown UDP control message");
          default:
            return NUM2INT(type);
        }
    }
    else {
        switch (level) {
          case SOL_SOCKET:
            return constant_arg(type, rsock_scm_optname_to_int, "unknown UNIX control message");
          default:
            return NUM2INT(type);
        }
    }
}

int
rsock_shutdown_how_arg(VALUE how)
{
    /* convert SHUT_RD, SHUT_WR, SHUT_RDWR. */
    return constant_arg(how, rsock_shutdown_how_to_int, "unknown shutdown argument");
}

/*
 * Socket::Constants module
 */
void
rsock_init_socket_constants(void)
{
    /* constants */
    init_constants();
}
