require 'mkmf'

$INCFLAGS << " -I$(topdir) -I$(top_srcdir)"

case RUBY_PLATFORM
when /(ms|bcc)win(32|64)|mingw/
  test_func = "WSACleanup"
  have_library("ws2_32", "WSACleanup")
when /cygwin/
  test_func = "socket"
when /beos/
  test_func = "socket"
  have_library("net", "socket")
when /haiku/
  test_func = "socket"
  have_library("network", "socket")
when /i386-os2_emx/
  test_func = "socket"
  have_library("socket", "socket")
else
  test_func = "socket"
  have_library("nsl", "t_open")
  have_library("socket", "socket")
end

unless $mswin or $bccwin or $mingw
  headers = %w<sys/types.h netdb.h string.h sys/socket.h netinet/in.h>
end
if /solaris/ =~ RUBY_PLATFORM and !try_compile("")
  # bug of gcc 3.0 on Solaris 8 ?
  headers << "sys/feature_tests.h"
end
if have_header("arpa/inet.h")
  headers << "arpa/inet.h"
end

ipv6 = false
default_ipv6 = /mswin|cygwin|beos|haiku/ !~ RUBY_PLATFORM
if enable_config("ipv6", default_ipv6)
  if checking_for("ipv6") {try_link(<<EOF)}
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#endif
int
main(void)
{
  socket(AF_INET6, SOCK_STREAM, 0);
  return 0;
}
EOF
    $defs << "-DENABLE_IPV6" << "-DINET6"
    ipv6 = true
  end
end

if ipv6
  if $mingw
    $CPPFLAGS << " -D_WIN32_WINNT=0x501" unless $CPPFLAGS.include?("_WIN32_WINNT")
  end
  ipv6lib = nil
  class << (fmt = "unknown")
    def %(s) s || self end
  end
  idirs, ldirs = dir_config("inet6", %w[/usr/inet6 /usr/local/v6].find {|d| File.directory?(d)})
  checking_for("ipv6 type", fmt) do
    if have_macro("IPV6_INRIA_VERSION", "netinet/in.h")
      "inria"
    elsif have_macro("__KAME__", "netinet/in.h")
      have_library(ipv6lib = "inet6")
      "kame"
    elsif have_macro("_TOSHIBA_INET6", "sys/param.h")
      have_library(ipv6lib = "inet6") and "toshiba"
    elsif have_macro("__V6D__", "sys/v6config.h")
      have_library(ipv6lib = "v6") and "v6d"
    elsif have_macro("_ZETA_MINAMI_INET6", "sys/param.h")
      have_library(ipv6lib = "inet6") and "zeta"
    elsif ipv6lib = with_config("ipv6-lib")
      warn <<EOS
--with-ipv6-lib and --with-ipv6-libdir option will be obsolete, use
--with-inet6lib and --with-inet6-{include,lib} options instead.
EOS
      find_library(ipv6lib, nil, with_config("ipv6-libdir", ldirs)) and
        ipv6lib
    elsif have_library("inet6")
      "inet6"
    end
  end or not ipv6lib or abort <<EOS

Fatal: no #{ipv6lib} library found.  cannot continue.
You need to fetch lib#{ipv6lib}.a from appropriate
ipv6 kit and compile beforehand.
EOS
end

if have_struct_member("struct sockaddr_in", "sin_len", headers)
  $defs[-1] = "-DHAVE_SIN_LEN"
end

#   doug's fix, NOW add -Dss_family... only if required!
doug = proc {have_struct_member("struct sockaddr_storage", "ss_family", headers)}
if (doug[] or
    with_cppflags($CPPFLAGS + " -Dss_family=__ss_family", &doug))
  $defs[-1] = "-DHAVE_SOCKADDR_STORAGE"
  doug = proc {have_struct_member("struct sockaddr_storage", "ss_len", headers)}
  doug[] or with_cppflags($CPPFLAGS + " -Dss_len=__ss_len", &doug)
end

if have_struct_member("struct sockaddr", "sa_len", headers)
  $defs[-1] = "-DHAVE_SA_LEN "
end

have_header("netinet/tcp.h") if /cygwin/ !~ RUBY_PLATFORM # for cygwin 1.1.5
have_header("netinet/udp.h")

if !have_macro("IPPROTO_IPV6", headers) && have_const("IPPROTO_IPV6", headers)
  IO.read(File.join(File.dirname(__FILE__), "mkconstants.rb")).sub(/\A.*^__END__$/m, '').split(/\r?\n/).grep(/\AIPPROTO_\w*/){$&}.each {|name|
    have_const(name, headers) unless $defs.include?("-DHAVE_CONST_#{name.upcase}")
  }
end

if have_func("sendmsg") | have_func("recvmsg")
  have_struct_member('struct msghdr', 'msg_control', ['sys/types.h', 'sys/socket.h'])
  have_struct_member('struct msghdr', 'msg_accrights', ['sys/types.h', 'sys/socket.h'])
end

if checking_for("recvmsg() with MSG_PEEK allocate file descriptors") {try_run(cpp_include(headers) + <<'EOF')}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int ps[2], sv[2];
    int ret;
    ssize_t ss;
    int s_fd, r_fd;
    struct msghdr s_msg, r_msg;
    union {
        struct cmsghdr hdr;
        char dummy[CMSG_SPACE(sizeof(int))];
    } s_cmsg, r_cmsg;
    struct iovec s_iov, r_iov;
    char s_buf[1], r_buf[1];
    struct stat s_statbuf, r_statbuf;

    ret = pipe(ps);
    if (ret == -1) { perror("pipe"); exit(EXIT_FAILURE); }

    s_fd = ps[0];

    ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, sv);
    if (ret == -1) { perror("socketpair"); exit(EXIT_FAILURE); }

    s_msg.msg_name = NULL;
    s_msg.msg_namelen = 0;
    s_msg.msg_iov = &s_iov;
    s_msg.msg_iovlen = 1;
    s_msg.msg_control = &s_cmsg;
    s_msg.msg_controllen = CMSG_SPACE(sizeof(int));;
    s_msg.msg_flags = 0;

    s_iov.iov_base = &s_buf;
    s_iov.iov_len = sizeof(s_buf);

    s_buf[0] = 'a';

    s_cmsg.hdr.cmsg_len = CMSG_LEN(sizeof(int));
    s_cmsg.hdr.cmsg_level = SOL_SOCKET;
    s_cmsg.hdr.cmsg_type = SCM_RIGHTS;
    memcpy(CMSG_DATA(&s_cmsg.hdr), (char *)&s_fd, sizeof(int));

    ss = sendmsg(sv[0], &s_msg, 0);
    if (ss == -1) { perror("sendmsg"); exit(EXIT_FAILURE); }

    r_msg.msg_name = NULL;
    r_msg.msg_namelen = 0;
    r_msg.msg_iov = &r_iov;
    r_msg.msg_iovlen = 1;
    r_msg.msg_control = &r_cmsg;
    r_msg.msg_controllen = CMSG_SPACE(sizeof(int));
    r_msg.msg_flags = 0;

    r_iov.iov_base = &r_buf;
    r_iov.iov_len = sizeof(r_buf);

    r_buf[0] = '0';

    memset(&r_cmsg, 0xff, CMSG_SPACE(sizeof(int)));

    ss = recvmsg(sv[1], &r_msg, MSG_PEEK);
    if (ss == -1) { perror("recvmsg"); exit(EXIT_FAILURE); }

    if (ss != 1) {
        fprintf(stderr, "unexpected return value from recvmsg: %ld\n", (long)ss);
        exit(EXIT_FAILURE);
    }
    if (r_buf[0] != 'a') {
        fprintf(stderr, "unexpected return data from recvmsg: 0x%02x\n", r_buf[0]);
        exit(EXIT_FAILURE);
    }

    if (r_msg.msg_controllen < CMSG_LEN(sizeof(int))) {
        fprintf(stderr, "unexpected: r_msg.msg_controllen < CMSG_LEN(sizeof(int)) not hold: %ld\n",
                (long)r_msg.msg_controllen);
        exit(EXIT_FAILURE);
    }
    if (r_cmsg.hdr.cmsg_len < CMSG_LEN(sizeof(int))) {
        fprintf(stderr, "unexpected: r_cmsg.hdr.cmsg_len < CMSG_LEN(sizeof(int)) not hold: %ld\n",
                (long)r_cmsg.hdr.cmsg_len);
        exit(EXIT_FAILURE);
    }
    memcpy((char *)&r_fd, CMSG_DATA(&r_cmsg.hdr), sizeof(int));

    if (r_fd < 0) {
        fprintf(stderr, "negative r_fd: %d\n", r_fd);
        exit(EXIT_FAILURE);
    }

    if (r_fd == s_fd) {
        fprintf(stderr, "r_fd and s_fd is same: %d\n", r_fd);
        exit(EXIT_FAILURE);
    }

    ret = fstat(s_fd, &s_statbuf);
    if (ret == -1) { perror("fstat(s_fd)"); exit(EXIT_FAILURE); }

    ret = fstat(r_fd, &r_statbuf);
    if (ret == -1) { perror("fstat(r_fd)"); exit(EXIT_FAILURE); }

    if (s_statbuf.st_dev != r_statbuf.st_dev ||
        s_statbuf.st_ino != r_statbuf.st_ino) {
        fprintf(stderr, "dev/ino doesn't match: s_fd:%ld/%ld r_fd:%ld/%ld\n",
                (long)s_statbuf.st_dev, (long)s_statbuf.st_ino,
                (long)r_statbuf.st_dev, (long)r_statbuf.st_ino);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
EOF
  $defs << "-DFD_PASSING_WORK_WITH_RECVMSG_MSG_PEEK"
end

getaddr_info_ok = (enable_config("wide-getaddrinfo") && :wide) ||
  (checking_for("wide getaddrinfo") {try_run(<<EOF)} && :os)
#{cpp_include(headers)}
#include <stdlib.h>

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif

int
main(void)
{
  int passive, gaierr, inet4 = 0, inet6 = 0;
  struct addrinfo hints, *ai, *aitop;
  char straddr[INET6_ADDRSTRLEN], strport[16];
#ifdef _WIN32
  WSADATA retdata;

  WSAStartup(MAKEWORD(2, 0), &retdata);
#endif

  for (passive = 0; passive <= 1; passive++) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = passive ? AI_PASSIVE : 0;
    hints.ai_socktype = SOCK_STREAM;
    if ((gaierr = getaddrinfo(NULL, "54321", &hints, &aitop)) != 0) {
      (void)gai_strerror(gaierr);
      goto bad;
    }
    for (ai = aitop; ai; ai = ai->ai_next) {
      if (ai->ai_family == AF_LOCAL) continue;
      if (ai->ai_addr == NULL)
        goto bad;
#if defined(_AIX)
      if (ai->ai_family == AF_INET6 && passive) {
        inet6++;
        continue;
      }
      ai->ai_addr->sa_len = ai->ai_addrlen;
      ai->ai_addr->sa_family = ai->ai_family;
#endif
      if (ai->ai_addrlen == 0 ||
          getnameinfo(ai->ai_addr, ai->ai_addrlen,
                      straddr, sizeof(straddr), strport, sizeof(strport),
                      NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
        goto bad;
      }
      if (strcmp(strport, "54321") != 0) {
        goto bad;
      }
      switch (ai->ai_family) {
      case AF_INET:
        if (passive) {
          if (strcmp(straddr, "0.0.0.0") != 0) {
            goto bad;
          }
        } else {
          if (strcmp(straddr, "127.0.0.1") != 0) {
            goto bad;
          }
        }
        inet4++;
        break;
      case AF_INET6:
        if (passive) {
          if (strcmp(straddr, "::") != 0) {
            goto bad;
          }
        } else {
          if (strcmp(straddr, "::1") != 0) {
            goto bad;
          }
        }
        inet6++;
        break;
      case AF_UNSPEC:
        goto bad;
        break;
      default:
        /* another family support? */
        break;
      }
    }
  }

  if (!(inet4 == 0 || inet4 == 2))
    goto bad;
  if (!(inet6 == 0 || inet6 == 2))
    goto bad;

  if (aitop)
    freeaddrinfo(aitop);
  return EXIT_SUCCESS;

 bad:
  if (aitop)
    freeaddrinfo(aitop);
  return EXIT_FAILURE;
}
EOF
if ipv6 and not getaddr_info_ok
  abort <<EOS

Fatal: --enable-ipv6 is specified, and your OS seems to support IPv6 feature.
But your getaddrinfo() and getnameinfo() are appeared to be broken.  Sorry,
you cannot compile IPv6 socket classes with broken these functions.
You can try --enable-wide-getaddrinfo.
EOS
end

case with_config("lookup-order-hack", "UNSPEC")
when "INET"
  $defs << "-DLOOKUP_ORDER_HACK_INET"
when "INET6"
  $defs << "-DLOOKUP_ORDER_HACK_INET6"
when "UNSPEC"
  # nothing special
else
  abort <<EOS

Fatal: invalid value for --with-lookup-order-hack (expected INET, INET6 or UNSPEC)
EOS
end

have_type("struct addrinfo", headers)
have_func("freehostent")
have_func("freeaddrinfo")
if /haiku/ !~ RUBY_PLATFORM and have_func("gai_strerror")
  if checking_for("gai_strerror() returns const pointer") {!try_compile(<<EOF)}
#{cpp_include(headers)}
#include <stdlib.h>
void
conftest_gai_strerror_is_const()
{
    *gai_strerror(0) = 0;
}
EOF
    $defs << "-DGAI_STRERROR_CONST"
  end
end

have_func("accept4")

$objs = [
  "init.#{$OBJEXT}",
  "constants.#{$OBJEXT}",
  "basicsocket.#{$OBJEXT}",
  "socket.#{$OBJEXT}",
  "ipsocket.#{$OBJEXT}",
  "tcpsocket.#{$OBJEXT}",
  "tcpserver.#{$OBJEXT}",
  "sockssocket.#{$OBJEXT}",
  "udpsocket.#{$OBJEXT}",
  "unixsocket.#{$OBJEXT}",
  "unixserver.#{$OBJEXT}",
  "option.#{$OBJEXT}",
  "ancdata.#{$OBJEXT}",
  "raddrinfo.#{$OBJEXT}"
]

if getaddr_info_ok == :wide or
    !have_func("getnameinfo", headers) or !have_func("getaddrinfo", headers)
  if have_struct_member("struct in6_addr", "s6_addr8", headers)
    $defs[-1] = "s6_addr=s6_addr8"
  end
  if ipv6 == "kame" && have_struct_member("struct in6_addr", "s6_addr32", headers)
    $defs[-1] = "-DFAITH"
  end
  $CPPFLAGS="-I. "+$CPPFLAGS
  $objs += ["getaddrinfo.#{$OBJEXT}"]
  $objs += ["getnameinfo.#{$OBJEXT}"]
  $defs << "-DGETADDRINFO_EMU"
end

have_func('inet_ntop(0, (const void *)0, (char *)0, 0)') or
  have_func("inet_ntoa(*(struct in_addr *)NULL)")
have_func('inet_pton(0, "", (void *)0)') or have_func('inet_aton("", (struct in_addr *)0)')
have_func('getservbyport(0, "")')
have_header("arpa/nameser.h")
have_header("resolv.h")

have_header("ifaddrs.h")
have_func("getifaddrs")
have_header("sys/ioctl.h")
have_header("sys/sockio.h")
have_header("net/if.h", headers)

have_header("sys/param.h", headers)
have_header("sys/ucred.h", headers)

unless have_type("socklen_t", headers)
  $defs << "-Dsocklen_t=int"
end

have_header("sys/un.h")
have_header("sys/uio.h")
have_type("struct in_pktinfo", headers) {|src|
  src.sub(%r'^/\*top\*/', '\&'"\n#if defined(IPPROTO_IP) && defined(IP_PKTINFO)") <<
  "#else\n" << "#error\n" << ">>>>>> no in_pktinfo <<<<<<\n" << "#endif\n"
} and have_struct_member("struct in_pktinfo", "ipi_spec_dst", headers)
have_type("struct in6_pktinfo", headers) {|src|
  src.sub(%r'^/\*top\*/', '\&'"\n#if defined(IPPROTO_IPV6) && defined(IPV6_PKTINFO)") <<
  "#else\n" << "#error\n" << ">>>>>> no in6_pktinfo <<<<<<\n" << "#endif\n"
}

have_type("struct sockcred", headers)
have_type("struct cmsgcred", headers)

have_func("getpeereid")

have_header("ucred.h", headers)
have_func("getpeerucred")

have_func("if_indextoname")

have_type("struct ip_mreq", headers) # 4.4BSD
have_type("struct ip_mreqn", headers) # Linux 2.4
have_type("struct ipv6_mreq", headers) # RFC 3493

# workaround for recent Windows SDK
$defs << "-DIPPROTO_IPV6=IPPROTO_IPV6" if $defs.include?("-DHAVE_CONST_IPPROTO_IPV6") && !have_macro("IPPROTO_IPV6")

$distcleanfiles << "constants.h" << "constdefs.*"

if have_func(test_func)
  have_func("hsterror")
  have_func("getipnodebyname") or have_func("gethostbyname2")
  if !have_func("socketpair(0, 0, 0, 0)") and have_func("rb_w32_socketpair(0, 0, 0, 0)")
    $defs << "-Dsocketpair(a,b,c,d)=rb_w32_socketpair((a),(b),(c),(d))"
    $defs << "-DHAVE_SOCKETPAIR"
  end
  unless have_func("gethostname((char *)0, 0)")
    have_func("uname")
  end
  if enable_config("socks", ENV["SOCKS_SERVER"])
    if have_library("socks5", "SOCKSinit")
      $defs << "-DSOCKS5" << "-DSOCKS"
    elsif have_library("socks", "Rconnect")
      $defs << "-DSOCKS"
    end
  end
  create_makefile("socket")
end
