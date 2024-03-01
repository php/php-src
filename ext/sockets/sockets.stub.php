<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue AF_UNIX
 */
const AF_UNIX = UNKNOWN;
/**
 * @var int
 * @cvalue AF_INET
 */
const AF_INET = UNKNOWN;
#ifdef HAVE_IPV6
/**
 * @var int
 * @cvalue AF_INET6
 */
const AF_INET6 = UNKNOWN;
#endif
#ifdef AF_DIVERT
/**
 * @var int
 * @cvalue AF_DIVERT
 */
const AF_DIVERT = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue SOCK_STREAM
 */
const SOCK_STREAM = UNKNOWN;
/**
 * @var int
 * @cvalue SOCK_DGRAM
 */
const SOCK_DGRAM = UNKNOWN;
/**
 * @var int
 * @cvalue SOCK_RAW
 */
const SOCK_RAW = UNKNOWN;
/**
 * @var int
 * @cvalue SOCK_SEQPACKET
 */
const SOCK_SEQPACKET = UNKNOWN;
#ifdef SOCK_RDM
/**
 * @var int
 * @cvalue SOCK_RDM
 */
const SOCK_RDM = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue MSG_OOB
 */
const MSG_OOB = UNKNOWN;
/**
 * @var int
 * @cvalue MSG_WAITALL
 */
const MSG_WAITALL = UNKNOWN;
/**
 * @var int
 * @cvalue MSG_CTRUNC
 */
const MSG_CTRUNC = UNKNOWN;
/**
 * @var int
 * @cvalue MSG_TRUNC
 */
const MSG_TRUNC = UNKNOWN;
/**
 * @var int
 * @cvalue MSG_PEEK
 */
const MSG_PEEK = UNKNOWN;
/**
 * @var int
 * @cvalue MSG_DONTROUTE
 */
const MSG_DONTROUTE = UNKNOWN;
#ifdef MSG_EOR
/**
 * @var int
 * @cvalue MSG_EOR
 */
const MSG_EOR = UNKNOWN;
#endif
#ifdef MSG_EOF
/**
 * @var int
 * @cvalue MSG_EOF
 */
const MSG_EOF = UNKNOWN;
#endif

#ifdef MSG_CONFIRM
/**
 * @var int
 * @cvalue MSG_CONFIRM
 */
const MSG_CONFIRM = UNKNOWN;
#endif
#ifdef MSG_ERRQUEUE
/**
 * @var int
 * @cvalue MSG_ERRQUEUE
 */
const MSG_ERRQUEUE = UNKNOWN;
#endif
#ifdef MSG_NOSIGNAL
/**
 * @var int
 * @cvalue MSG_NOSIGNAL
 */
const MSG_NOSIGNAL = UNKNOWN;
#endif
#ifdef MSG_DONTWAIT
/**
 * @var int
 * @cvalue MSG_DONTWAIT
 */
const MSG_DONTWAIT = UNKNOWN;
#endif
#ifdef MSG_MORE
/**
 * @var int
 * @cvalue MSG_MORE
 */
const MSG_MORE = UNKNOWN;
#endif
#ifdef MSG_WAITFORONE
/**
 * @var int
 * @cvalue MSG_WAITFORONE
 */
const MSG_WAITFORONE = UNKNOWN;
#endif
#ifdef MSG_CMSG_CLOEXEC
/**
 * @var int
 * @cvalue MSG_CMSG_CLOEXEC
 */
const MSG_CMSG_CLOEXEC = UNKNOWN;
#endif
#ifdef MSG_ZEROCOPY
/**
 * @var int
 * @cvalue MSG_ZEROCOPY
 */
const MSG_ZEROCOPY = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue SO_DEBUG
 */
const SO_DEBUG = UNKNOWN;
/**
 * @var int
 * @cvalue SO_REUSEADDR
 */
const SO_REUSEADDR = UNKNOWN;
#ifdef SO_REUSEPORT
/**
 * @var int
 * @cvalue SO_REUSEPORT
 */
const SO_REUSEPORT = UNKNOWN;
#endif
#ifdef SO_REUSEPORT_LB
/**
 * @var int
 * @cvalue SO_REUSEPORT_LB
 */
const SO_REUSEPORT_LB = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue SO_KEEPALIVE
 */
const SO_KEEPALIVE = UNKNOWN;
/**
 * @var int
 * @cvalue SO_DONTROUTE
 */
const SO_DONTROUTE = UNKNOWN;
/**
 * @var int
 * @cvalue SO_LINGER
 */
const SO_LINGER = UNKNOWN;
/**
 * @var int
 * @cvalue SO_BROADCAST
 */
const SO_BROADCAST = UNKNOWN;
/**
 * @var int
 * @cvalue SO_OOBINLINE
 */
const SO_OOBINLINE = UNKNOWN;
/**
 * @var int
 * @cvalue SO_SNDBUF
 */
const SO_SNDBUF = UNKNOWN;
/**
 * @var int
 * @cvalue SO_RCVBUF
 */
const SO_RCVBUF = UNKNOWN;
/**
 * @var int
 * @cvalue SO_SNDLOWAT
 */
const SO_SNDLOWAT = UNKNOWN;
/**
 * @var int
 * @cvalue SO_RCVLOWAT
 */
const SO_RCVLOWAT = UNKNOWN;
/**
 * @var int
 * @cvalue SO_SNDTIMEO
 */
const SO_SNDTIMEO = UNKNOWN;
/**
 * @var int
 * @cvalue SO_RCVTIMEO
 */
const SO_RCVTIMEO = UNKNOWN;
/**
 * @var int
 * @cvalue SO_TYPE
 */
const SO_TYPE = UNKNOWN;
#ifdef SO_FAMILY
/**
 * @var int
 * @cvalue SO_FAMILY
 */
const SO_FAMILY = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue SO_ERROR
 */
const SO_ERROR = UNKNOWN;
#ifdef SO_BINDTODEVICE
/**
 * @var int
 * @cvalue SO_BINDTODEVICE
 */
const SO_BINDTODEVICE = UNKNOWN;
#endif
#ifdef SO_USER_COOKIE
/**
 * @var int
 * @cvalue SO_LABEL
 */
const SO_LABEL = UNKNOWN;
/**
 * @var int
 * @cvalue SO_PEERLABEL
 */
const SO_PEERLABEL = UNKNOWN;
/**
 * @var int
 * @cvalue SO_LISTENQLIMIT
 */
const SO_LISTENQLIMIT = UNKNOWN;
/**
 * @var int
 * @cvalue SO_LISTENQLEN
 */
const SO_LISTENQLEN = UNKNOWN;
/**
 * @var int
 * @cvalue SO_USER_COOKIE
 */
const SO_USER_COOKIE = UNKNOWN;
#endif
#ifdef SO_SETFIB
/**
 * @var int
 * @cvalue SO_SETFIB
 */
const SO_SETFIB = UNKNOWN;
#endif
#ifdef SO_ACCEPTFILTER
/**
 * @var int
 * @cvalue SO_ACCEPTFILTER
 */
const SO_ACCEPTFILTER = UNKNOWN;
#endif
#ifdef SO_RERROR
/**
 * @var int
 * @cvalue SO_RERROR
 */
const SO_RERROR = UNKNOWN;
#endif
#ifdef SO_SOPLICE
/**
 * @var int
 * @cvalue SO_SPLICE
 */
const SO_SPLICE = UNKNOWN;
#endif
#ifdef SO_ZEROIZE
/**
 * @var int
 * @cvalue SO_ZEROIZE
 */
const SO_ZEROIZE = UNKNOWN;
#endif
#ifdef SOL_FILTER
/**
 * @var int
 * @cvalue SOL_FILTER
 */
const SOL_FILTER = UNKNOWN;
/**
 * @var int
 * @cvalue FIL_ATTACH
 */
const FIL_ATTACH = UNKNOWN;
/**
 * @var int
 * @cvalue FIL_DETACH
 */
const FIL_DETACH = UNKNOWN;
#endif
#ifdef SO_DONTTRUNC
/**
 * @var int
 * @cvalue SO_DONTTRUNC
 */
const SO_DONTTRUNC = UNKNOWN;
#endif
#ifdef SO_WANTMORE
/**
 * @var int
 * @cvalue SO_WANTMORE
 */
const SO_WANTMORE = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue SOL_SOCKET
 */
const SOL_SOCKET = UNKNOWN;
/**
 * @var int
 * @cvalue SOMAXCONN
 */
const SOMAXCONN = UNKNOWN;
#ifdef SO_MARK
/**
 * @var int
 * @cvalue SO_MARK
 */
const SO_MARK = UNKNOWN;
#endif
#ifdef SO_RTABLE
/**
 * @var int
 * @cvalue SO_RTABLE
 */
const SO_RTABLE = UNKNOWN;
#endif
#ifdef SO_INCOMING_CPU
/**
 * @var int
 * @cvalue SO_INCOMING_CPU
 */
const SO_INCOMING_CPU = UNKNOWN;
#endif
#ifdef SO_MEMINFO
/**
 * @var int
 * @cvalue SO_MEMINFO
 */
const SO_MEMINFO = UNKNOWN;
#endif
#ifdef SO_BPF_EXTENSIONS
/**
 * @var int
 * @cvalue SO_BPF_EXTENSIONS
 */
const SO_BPF_EXTENSIONS = UNKNOWN;
#endif
#ifdef SKF_AD_OFF
/**
 * @var int
 * @cvalue SKF_AD_OFF
 */
const SKF_AD_OFF = UNKNOWN;
#endif
#ifdef SKF_AD_PROTOCOL
/**
 * @var int
 * @cvalue SKF_AD_PROTOCOL
 */
const SKF_AD_PROTOCOL = UNKNOWN;
#endif
#ifdef SKF_AD_PKTTYPE
/**
 * @var int
 * @cvalue SKF_AD_PKTTYPE
 */
const SKF_AD_PKTTYPE = UNKNOWN;
#endif
#ifdef SKF_AD_IFINDEX
/**
 * @var int
 * @cvalue SKF_AD_IFINDEX
 */
const SKF_AD_IFINDEX = UNKNOWN;
#endif
#ifdef SKF_AD_NLATTR
/**
 * @var int
 * @cvalue SKF_AD_NLATTR
 */
const SKF_AD_NLATTR = UNKNOWN;
#endif
#ifdef SKF_AD_NLATTR_NEST
/**
 * @var int
 * @cvalue SKF_AD_NLATTR_NEST
 */
const SKF_AD_NLATTR_NEST = UNKNOWN;
#endif
#ifdef SKF_AD_MARK
/**
 * @var int
 * @cvalue SKF_AD_MARK
 */
const SKF_AD_MARK = UNKNOWN;
#endif
#ifdef SKF_AD_QUEUE
/**
 * @var int
 * @cvalue SKF_AD_QUEUE
 */
const SKF_AD_QUEUE = UNKNOWN;
#endif
#ifdef SKF_AD_HATYPE
/**
 * @var int
 * @cvalue SKF_AD_HATYPE
 */
const SKF_AD_HATYPE = UNKNOWN;
#endif
#ifdef SKF_AD_RXHASH
/**
 * @var int
 * @cvalue SKF_AD_RXHASH
 */
const SKF_AD_RXHASH = UNKNOWN;
#endif
#ifdef SKF_AD_CPU
/**
 * @var int
 * @cvalue SKF_AD_CPU
 */
const SKF_AD_CPU = UNKNOWN;
#endif
#ifdef SKF_AD_ALU_XOR_X
/**
 * @var int
 * @cvalue SKF_AD_ALU_XOR_X
 */
const SKF_AD_ALU_XOR_X = UNKNOWN;
#endif
#ifdef SKF_AD_VLAN_TAG
/**
 * @var int
 * @cvalue SKF_AD_VLAN_TAG
 */
const SKF_AD_VLAN_TAG = UNKNOWN;
#endif
#ifdef SKF_AD_VLAN_TAG_PRESENT
/**
 * @var int
 * @cvalue SKF_AD_VLAN_TAG_PRESENT
 */
const SKF_AD_VLAN_TAG_PRESENT = UNKNOWN;
#endif
#ifdef SKF_AD_PAY_OFFSET
/**
 * @var int
 * @cvalue SKF_AD_PAY_OFFSET
 */
const SKF_AD_PAY_OFFSET = UNKNOWN;
#endif
#ifdef SKF_AD_RANDOM
/**
 * @var int
 * @cvalue SKF_AD_RANDOM
 */
const SKF_AD_RANDOM = UNKNOWN;
#endif
#ifdef SKF_AD_VLAN_TPID
/**
 * @var int
 * @cvalue SKF_AD_VLAN_TPID
 */
const SKF_AD_VLAN_TPID = UNKNOWN;
#endif
#ifdef SKF_AD_MAX
/**
 * @var int
 * @cvalue SKF_AD_MAX
 */
const SKF_AD_MAX = UNKNOWN;
#endif

#ifdef TCP_CONGESTION
/**
 * @var int
 * @cvalue TCP_CONGESTION
 */
const TCP_CONGESTION = UNKNOWN;
#endif
#ifdef SO_ZEROCOPY
/**
 * @var int
 * @cvalue SO_ZEROCOPY
 */
const SO_ZEROCOPY = UNKNOWN;
#endif
#ifdef TCP_NODELAY
/**
 * @var int
 * @cvalue TCP_NODELAY
 */
const TCP_NODELAY = UNKNOWN;
#endif
#ifdef TCP_NOTSENT_LOWAT
/**
 * @var int
 * @cvalue TCP_NOTSENT_LOWAT
 */
const TCP_NOTSENT_LOWAT = UNKNOWN;
#endif
#ifdef TCP_DEFER_ACCEPT
/**
 * @var int
 * @cvalue TCP_DEFER_ACCEPT
 */
const TCP_DEFER_ACCEPT = UNKNOWN;
#endif
#ifdef TCP_KEEPALIVE
/**
 * @var int
 * @cvalue TCP_KEEPALIVE
 */
const TCP_KEEPALIVE = UNKNOWN;
#endif
#ifdef TCP_KEEPIDLE
/**
 * @var int
 * @cvalue TCP_KEEPIDLE
 */
const TCP_KEEPIDLE = UNKNOWN;
/**
 * @var int
 * @cvalue TCP_KEEPINTVL
 */
const TCP_KEEPINTVL = UNKNOWN;
/**
 * @var int
 * @cvalue TCP_KEEPCNT
 */
const TCP_KEEPCNT = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue PHP_NORMAL_READ
 */
const PHP_NORMAL_READ = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_BINARY_READ
 */
const PHP_BINARY_READ = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_MCAST_JOIN_GROUP
 */
const MCAST_JOIN_GROUP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_MCAST_LEAVE_GROUP
 */
const MCAST_LEAVE_GROUP = UNKNOWN;
#ifdef HAS_MCAST_EXT
/**
 * @var int
 * @cvalue PHP_MCAST_BLOCK_SOURCE
 */
const MCAST_BLOCK_SOURCE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_MCAST_UNBLOCK_SOURCE
 */
const MCAST_UNBLOCK_SOURCE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_MCAST_JOIN_SOURCE_GROUP
 */
const MCAST_JOIN_SOURCE_GROUP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_MCAST_LEAVE_SOURCE_GROUP
 */
const MCAST_LEAVE_SOURCE_GROUP = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue IP_MULTICAST_IF
 */
const IP_MULTICAST_IF = UNKNOWN;
/**
 * @var int
 * @cvalue IP_MULTICAST_TTL
 */
const IP_MULTICAST_TTL = UNKNOWN;
/**
 * @var int
 * @cvalue IP_MULTICAST_LOOP
 */
const IP_MULTICAST_LOOP = UNKNOWN;
#ifdef IP_BIND_ADDRESS_NO_PORT
/**
 * @var int
 * @cvalue IP_BIND_ADDRESS_NO_PORT
 */
const IP_BIND_ADDRESS_NO_PORT = UNKNOWN;
#endif
#ifdef HAVE_IPV6
/**
 * @var int
 * @cvalue IPV6_MULTICAST_IF
 */
const IPV6_MULTICAST_IF = UNKNOWN;
/**
 * @var int
 * @cvalue IPV6_MULTICAST_HOPS
 */
const IPV6_MULTICAST_HOPS = UNKNOWN;
/**
 * @var int
 * @cvalue IPV6_MULTICAST_LOOP
 */
const IPV6_MULTICAST_LOOP = UNKNOWN;
#endif

#ifdef IPV6_V6ONLY
/**
 * @var int
 * @cvalue IPV6_V6ONLY
 */
const IPV6_V6ONLY = UNKNOWN;
#endif

#ifdef EPERM
/**
 * Operation not permitted
 * @var int
 * @cvalue EPERM
 */
const SOCKET_EPERM = UNKNOWN;
#endif
#ifdef ENOENT
/**
 * No such file or directory
 * @var int
 * @cvalue ENOENT
 */
const SOCKET_ENOENT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EINTR
/**
 * Interrupted system call
 * @var int
 * @cvalue PHP_SOCKET_EINTR
 */
const SOCKET_EINTR = UNKNOWN;
#endif
#ifdef EIO
/**
 * I/O error
 * @var int
 * @cvalue EIO
 */
const SOCKET_EIO = UNKNOWN;
#endif
#ifdef ENXIO
/**
 * No such device or address
 * @var int
 * @cvalue ENXIO
 */
const SOCKET_ENXIO = UNKNOWN;
#endif
#ifdef E2BIG
/**
 * Arg list too long
 * @var int
 * @cvalue E2BIG
 */
const SOCKET_E2BIG = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EBADF
/**
 * Bad file number
 * @var int
 * @cvalue PHP_SOCKET_EBADF
 */
const SOCKET_EBADF = UNKNOWN;
#endif
#ifdef EAGAIN
/**
 * Try again
 * @var int
 * @cvalue EAGAIN
 */
const SOCKET_EAGAIN = UNKNOWN;
#endif
#ifdef ENOMEM
/**
 * Out of memory
 * @var int
 * @cvalue ENOMEM
 */
const SOCKET_ENOMEM = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EACCES
/**
 * Permission denied
 * @var int
 * @cvalue PHP_SOCKET_EACCES
 */
const SOCKET_EACCES = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EFAULT
/**
 * Bad address
 * @var int
 * @cvalue PHP_SOCKET_EFAULT
 */
const SOCKET_EFAULT = UNKNOWN;
#endif
#ifdef ENOTBLK
/**
 * Block device required
 * @var int
 * @cvalue ENOTBLK
 */
const SOCKET_ENOTBLK = UNKNOWN;
#endif
#ifdef EBUSY
/**
 * Device or resource busy
 * @var int
 * @cvalue EBUSY
 */
const SOCKET_EBUSY = UNKNOWN;
#endif
#ifdef EEXIST
/**
 * File exists
 * @var int
 * @cvalue EEXIST
 */
const SOCKET_EEXIST = UNKNOWN;
#endif
#ifdef EXDEV
/**
 * Cross-device link
 * @var int
 * @cvalue EXDEV
 */
const SOCKET_EXDEV = UNKNOWN;
#endif
#ifdef ENODEV
/**
 * No such device
 * @var int
 * @cvalue ENODEV
 */
const SOCKET_ENODEV = UNKNOWN;
#endif
#ifdef ENOTDIR
/**
 * Not a directory
 * @var int
 * @cvalue ENOTDIR
 */
const SOCKET_ENOTDIR = UNKNOWN;
#endif
#ifdef EISDIR
/**
 * Is a directory
 * @var int
 * @cvalue EISDIR
 */
const SOCKET_EISDIR = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EINVAL
/**
 * Invalid argument
 * @var int
 * @cvalue PHP_SOCKET_EINVAL
 */
const SOCKET_EINVAL = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENFILE
/**
 * File table overflow
 * @var int
 * @cvalue PHP_SOCKET_ENFILE
 */
const SOCKET_ENFILE = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EMFILE
/**
 * Too many open files
 * @var int
 * @cvalue PHP_SOCKET_EMFILE
 */
const SOCKET_EMFILE = UNKNOWN;
#endif
#ifdef ENOTTY
/**
 * Not a typewriter
 * @var int
 * @cvalue ENOTTY
 */
const SOCKET_ENOTTY = UNKNOWN;
#endif
#ifdef ENOSPC
/**
 * No space left on device
 * @var int
 * @cvalue ENOSPC
 */
const SOCKET_ENOSPC = UNKNOWN;
#endif
#ifdef ESPIPE
/**
 * Illegal seek
 * @var int
 * @cvalue ESPIPE
 */
const SOCKET_ESPIPE = UNKNOWN;
#endif
#ifdef EROFS
/**
 * Read-only file system
 * @var int
 * @cvalue EROFS
 */
const SOCKET_EROFS = UNKNOWN;
#endif
#ifdef EMLINK
/**
 * Too many links
 * @var int
 * @cvalue EMLINK
 */
const SOCKET_EMLINK = UNKNOWN;
#endif
#ifdef EPIPE
/**
 * Broken pipe
 * @var int
 * @cvalue EPIPE
 */
const SOCKET_EPIPE = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENAMETOOLONG
/**
 * File name too long
 * @var int
 * @cvalue PHP_SOCKET_ENAMETOOLONG
 */
const SOCKET_ENAMETOOLONG = UNKNOWN;
#endif
#ifdef ENOLCK
/**
 * No record locks available
 * @var int
 * @cvalue ENOLCK
 */
const SOCKET_ENOLCK = UNKNOWN;
#endif
#ifdef ENOSYS
/**
 * Function not implemented
 * @var int
 * @cvalue ENOSYS
 */
const SOCKET_ENOSYS = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENOTEMPTY
/**
 * Directory not empty
 * @var int
 * @cvalue PHP_SOCKET_ENOTEMPTY
 */
const SOCKET_ENOTEMPTY = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ELOOP
/**
 * Too many symbolic links encountered
 * @var int
 * @cvalue PHP_SOCKET_ELOOP
 */
const SOCKET_ELOOP = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EWOULDBLOCK
/**
 * Operation would block
 * @var int
 * @cvalue PHP_SOCKET_EWOULDBLOCK
 */
const SOCKET_EWOULDBLOCK = UNKNOWN;
#endif
#ifdef ENOMSG
/**
 * No message of desired type
 * @var int
 * @cvalue ENOMSG
 */
const SOCKET_ENOMSG = UNKNOWN;
#endif
#ifdef EIDRM
/**
 * Identifier removed
 * @var int
 * @cvalue EIDRM
 */
const SOCKET_EIDRM = UNKNOWN;
#endif
#ifdef ECHRNG
/**
 * Channel number out of range
 * @var int
 * @cvalue ECHRNG
 */
const SOCKET_ECHRNG = UNKNOWN;
#endif
#ifdef EL2NSYNC
/**
 * Level 2 not synchronized
 * @var int
 * @cvalue EL2NSYNC
 */
const SOCKET_EL2NSYNC = UNKNOWN;
#endif
#ifdef EL3HLT
/**
 * Level 3 halted
 * @var int
 * @cvalue EL3HLT
 */
const SOCKET_EL3HLT = UNKNOWN;
#endif
#ifdef EL3RST
/**
 * Level 3 reset
 * @var int
 * @cvalue EL3RST
 */
const SOCKET_EL3RST = UNKNOWN;
#endif
#ifdef ELNRNG
/**
 * Link number out of range
 * @var int
 * @cvalue ELNRNG
 */
const SOCKET_ELNRNG = UNKNOWN;
#endif
#ifdef EUNATCH
/**
 * Protocol driver not attached
 * @var int
 * @cvalue EUNATCH
 */
const SOCKET_EUNATCH = UNKNOWN;
#endif
#ifdef ENOCSI
/**
 * No CSI structure available
 * @var int
 * @cvalue ENOCSI
 */
const SOCKET_ENOCSI = UNKNOWN;
#endif
#ifdef EL2HLT
/**
 * Level 2 halted
 * @var int
 * @cvalue EL2HLT
 */
const SOCKET_EL2HLT = UNKNOWN;
#endif
#ifdef EBADE
/**
 * Invalid exchange
 * @var int
 * @cvalue EBADE
 */
const SOCKET_EBADE = UNKNOWN;
#endif
#ifdef EBADR
/**
 * Invalid request descriptor
 * @var int
 * @cvalue EBADR
 */
const SOCKET_EBADR = UNKNOWN;
#endif
#ifdef EXFULL
/**
 * Exchange full
 * @var int
 * @cvalue EXFULL
 */
const SOCKET_EXFULL = UNKNOWN;
#endif
#ifdef ENOANO
/**
 * No anode
 * @var int
 * @cvalue ENOANO
 */
const SOCKET_ENOANO = UNKNOWN;
#endif
#ifdef EBADRQC
/**
 * Invalid request code
 * @var int
 * @cvalue EBADRQC
 */
const SOCKET_EBADRQC = UNKNOWN;
#endif
#ifdef EBADSLT
/**
 * Invalid slot
 * @var int
 * @cvalue EBADSLT
 */
const SOCKET_EBADSLT = UNKNOWN;
#endif
#ifdef ENOSTR
/**
 * Device not a stream
 * @var int
 * @cvalue ENOSTR
 */
const SOCKET_ENOSTR = UNKNOWN;
#endif
#ifdef ENODATA
/**
 * No data available
 * @var int
 * @cvalue ENODATA
 */
const SOCKET_ENODATA = UNKNOWN;
#endif
#ifdef ETIME
/**
 * Timer expired
 * @var int
 * @cvalue ETIME
 */
const SOCKET_ETIME = UNKNOWN;
#endif
#ifdef ENOSR
/**
 * Out of streams resources
 * @var int
 * @cvalue ENOSR
 */
const SOCKET_ENOSR = UNKNOWN;
#endif
#ifdef ENONET
/**
 * Machine is not on the network
 * @var int
 * @cvalue ENONET
 */
const SOCKET_ENONET = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EREMOTE
/**
 * Object is remote
 * @var int
 * @cvalue PHP_SOCKET_EREMOTE
 */
const SOCKET_EREMOTE = UNKNOWN;
#endif
#ifdef ENOLINK
/**
 * Link has been severed
 * @var int
 * @cvalue ENOLINK
 */
const SOCKET_ENOLINK = UNKNOWN;
#endif
#ifdef EADV
/**
 * Advertise error
 * @var int
 * @cvalue EADV
 */
const SOCKET_EADV = UNKNOWN;
#endif
#ifdef ESRMNT
/**
 * Srmount error
 * @var int
 * @cvalue ESRMNT
 */
const SOCKET_ESRMNT = UNKNOWN;
#endif
#ifdef ECOMM
/**
 * Communication error on send
 * @var int
 * @cvalue ECOMM
 */
const SOCKET_ECOMM = UNKNOWN;
#endif
#ifdef EPROTO
/**
 * Protocol error
 * @var int
 * @cvalue EPROTO
 */
const SOCKET_EPROTO = UNKNOWN;
#endif
#ifdef EMULTIHOP
/**
 * Multihop attempted
 * @var int
 * @cvalue EMULTIHOP
 */
const SOCKET_EMULTIHOP = UNKNOWN;
#endif
#ifdef EBADMSG
/**
 * Not a data message
 * @var int
 * @cvalue EBADMSG
 */
const SOCKET_EBADMSG = UNKNOWN;
#endif
#ifdef ENOTUNIQ
/**
 * Name not unique on network
 * @var int
 * @cvalue ENOTUNIQ
 */
const SOCKET_ENOTUNIQ = UNKNOWN;
#endif
#ifdef EBADFD
/**
 * File descriptor in bad state
 * @var int
 * @cvalue EBADFD
 */
const SOCKET_EBADFD = UNKNOWN;
#endif
#ifdef EREMCHG
/**
 * Remote address changed
 * @var int
 * @cvalue EREMCHG
 */
const SOCKET_EREMCHG = UNKNOWN;
#endif
#ifdef ERESTART
/**
 * Interrupted system call should be restarted
 * @var int
 * @cvalue ERESTART
 */
const SOCKET_ERESTART = UNKNOWN;
#endif
#ifdef ESTRPIPE
/**
 * Streams pipe error
 * @var int
 * @cvalue ESTRPIPE
 */
const SOCKET_ESTRPIPE = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EUSERS
/**
 * Too many users
 * @var int
 * @cvalue PHP_SOCKET_EUSERS
 */
const SOCKET_EUSERS = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENOTSOCK
/**
 * Socket operation on non-socket
 * @var int
 * @cvalue PHP_SOCKET_ENOTSOCK
 */
const SOCKET_ENOTSOCK = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EDESTADDRREQ
/**
 * Destination address required
 * @var int
 * @cvalue PHP_SOCKET_EDESTADDRREQ
 */
const SOCKET_EDESTADDRREQ = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EMSGSIZE
/**
 * Message too long
 * @var int
 * @cvalue PHP_SOCKET_EMSGSIZE
 */
const SOCKET_EMSGSIZE = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EPROTOTYPE
/**
 * Protocol wrong type for socket
 * @var int
 * @cvalue PHP_SOCKET_EPROTOTYPE
 */
const SOCKET_EPROTOTYPE = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENOPROTOOPT
/**
 * Protocol not available
 * @var int
 * @cvalue PHP_SOCKET_ENOPROTOOPT
 */
const SOCKET_ENOPROTOOPT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EPROTONOSUPPORT
/**
 * Protocol not supported
 * @var int
 * @cvalue PHP_SOCKET_EPROTONOSUPPORT
 */
const SOCKET_EPROTONOSUPPORT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ESOCKTNOSUPPORT
/**
 * Socket type not supported
 * @var int
 * @cvalue PHP_SOCKET_ESOCKTNOSUPPORT
 */
const SOCKET_ESOCKTNOSUPPORT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EOPNOTSUPP
/**
 * Operation not supported on transport endpoint
 * @var int
 * @cvalue PHP_SOCKET_EOPNOTSUPP
 */
const SOCKET_EOPNOTSUPP = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EPFNOSUPPORT
/**
 * Protocol family not supported
 * @var int
 * @cvalue PHP_SOCKET_EPFNOSUPPORT
 */
const SOCKET_EPFNOSUPPORT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EAFNOSUPPORT
/**
 * Address family not supported by protocol
 * @var int
 * @cvalue PHP_SOCKET_EAFNOSUPPORT
 */
const SOCKET_EAFNOSUPPORT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EADDRINUSE
/**
 * Address already in use
 * @var int
 * @cvalue PHP_SOCKET_EADDRINUSE
 */
const SOCKET_EADDRINUSE = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EADDRNOTAVAIL
/**
 * Cannot assign requested address
 * @var int
 * @cvalue PHP_SOCKET_EADDRNOTAVAIL
 */
const SOCKET_EADDRNOTAVAIL = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENETDOWN
/**
 * Network is down
 * @var int
 * @cvalue PHP_SOCKET_ENETDOWN
 */
const SOCKET_ENETDOWN = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENETUNREACH
/**
 * Network is unreachable
 * @var int
 * @cvalue PHP_SOCKET_ENETUNREACH
 */
const SOCKET_ENETUNREACH = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENETRESET
/**
 * Network dropped connection because of reset
 * @var int
 * @cvalue PHP_SOCKET_ENETRESET
 */
const SOCKET_ENETRESET = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ECONNABORTED
/**
 * Software caused connection abort
 * @var int
 * @cvalue PHP_SOCKET_ECONNABORTED
 */
const SOCKET_ECONNABORTED = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ECONNRESET
/**
 * Connection reset by peer
 * @var int
 * @cvalue PHP_SOCKET_ECONNRESET
 */
const SOCKET_ECONNRESET = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENOBUFS
/**
 * No buffer space available
 * @var int
 * @cvalue PHP_SOCKET_ENOBUFS
 */
const SOCKET_ENOBUFS = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EISCONN
/**
 * Transport endpoint is already connected
 * @var int
 * @cvalue PHP_SOCKET_EISCONN
 */
const SOCKET_EISCONN = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ENOTCONN
/**
 * Transport endpoint is not connected
 * @var int
 * @cvalue PHP_SOCKET_ENOTCONN
 */
const SOCKET_ENOTCONN = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ESHUTDOWN
/**
 * Cannot send after transport endpoint shutdown
 * @var int
 * @cvalue PHP_SOCKET_ESHUTDOWN
 */
const SOCKET_ESHUTDOWN = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ETOOMANYREFS
/**
 * Too many references: cannot splice
 * @var int
 * @cvalue PHP_SOCKET_ETOOMANYREFS
 */
const SOCKET_ETOOMANYREFS = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ETIMEDOUT
/**
 * Connection timed out
 * @var int
 * @cvalue PHP_SOCKET_ETIMEDOUT
 */
const SOCKET_ETIMEDOUT = UNKNOWN;
#endif
#ifdef PHP_SOCKET_ECONNREFUSED
/**
 * Connection refused
 * @var int
 * @cvalue PHP_SOCKET_ECONNREFUSED
 */
const SOCKET_ECONNREFUSED = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EHOSTDOWN
/**
 * Host is down
 * @var int
 * @cvalue PHP_SOCKET_EHOSTDOWN
 */
const SOCKET_EHOSTDOWN = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EHOSTUNREACH
/**
 * No route to host
 * @var int
 * @cvalue PHP_SOCKET_EHOSTUNREACH
 */
const SOCKET_EHOSTUNREACH = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EALREADY
/**
 * Operation already in progress
 * @var int
 * @cvalue PHP_SOCKET_EALREADY
 */
const SOCKET_EALREADY = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EINPROGRESS
/**
 * Operation now in progress
 * @var int
 * @cvalue PHP_SOCKET_EINPROGRESS
 */
const SOCKET_EINPROGRESS = UNKNOWN;
#endif
#ifdef EISNAM
/**
 * Is a named type file
 * @var int
 * @cvalue EISNAM
 */
const SOCKET_EISNAM = UNKNOWN;
#endif
#ifdef EREMOTEIO
/**
 * Remote I/O error
 * @var int
 * @cvalue EREMOTEIO
 */
const SOCKET_EREMOTEIO = UNKNOWN;
#endif
#ifdef PHP_SOCKET_EDQUOT
/**
 * Quota exceeded
 * @var int
 * @cvalue PHP_SOCKET_EDQUOT
 */
const SOCKET_EDQUOT = UNKNOWN;
#endif
#ifdef ENOMEDIUM
/**
 * No medium found
 * @var int
 * @cvalue ENOMEDIUM
 */
const SOCKET_ENOMEDIUM = UNKNOWN;
#endif
#ifdef EMEDIUMTYPE
/**
 * Wrong medium type
 * @var int
 * @cvalue EMEDIUMTYPE
 */
const SOCKET_EMEDIUMTYPE = UNKNOWN;
#endif

#ifdef WIN32
/**
 * @var int
 * @cvalue WSAESTALE
 */
const SOCKET_ESTALE = UNKNOWN;
/**
 * @var int
 * @cvalue WSAEDISCON
 */
const SOCKET_EDISCON = UNKNOWN;
/**
 * @var int
 * @cvalue WSASYSNOTREADY
 */
const SOCKET_SYSNOTREADY = UNKNOWN;
/**
 * @var int
 * @cvalue WSAVERNOTSUPPORTED
 */
const SOCKET_VERNOTSUPPORTED = UNKNOWN;
/**
 * @var int
 * @cvalue WSANOTINITIALISED
 */
const SOCKET_NOTINITIALISED = UNKNOWN;
/**
 * @var int
 * @cvalue WSAHOST_NOT_FOUND
 */
const SOCKET_HOST_NOT_FOUND = UNKNOWN;
/**
 * @var int
 * @cvalue WSATRY_AGAIN
 */
const SOCKET_TRY_AGAIN = UNKNOWN;
/**
 * @var int
 * @cvalue WSANO_RECOVERY
 */
const SOCKET_NO_RECOVERY = UNKNOWN;
/**
 * @var int
 * @cvalue WSANO_DATA
 */
const SOCKET_NO_DATA = UNKNOWN;
/**
 * @var int
 * @cvalue WSANO_ADDRESS
 */
const SOCKET_NO_ADDRESS = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue IPPROTO_IP
 */
const IPPROTO_IP = UNKNOWN;
#ifdef HAVE_IPV6
/**
 * @var int
 * @cvalue IPPROTO_IPV6
 */
const IPPROTO_IPV6 = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue IPPROTO_TCP
 */
const SOL_TCP = UNKNOWN;
/**
 * @var int
 * @cvalue IPPROTO_UDP
 */
const SOL_UDP = UNKNOWN;
#ifdef IPPROTO_UDPLITE
/**
 * @var int
 * @cvalue IPPROTO_UDPLITE
 */
const SOL_UDPLITE = UNKNOWN;
#endif

#ifdef HAVE_IPV6
/**
 * @var int
 * @cvalue IPV6_UNICAST_HOPS
 */
const IPV6_UNICAST_HOPS = UNKNOWN;
#endif

/**
 * @var int
 * @cvalue AI_PASSIVE
 */
const AI_PASSIVE = UNKNOWN;
/**
 * @var int
 * @cvalue AI_CANONNAME
 */
const AI_CANONNAME = UNKNOWN;
/**
 * @var int
 * @cvalue AI_NUMERICHOST
 */
const AI_NUMERICHOST = UNKNOWN;
#ifdef AI_V4MAPPED
/**
 * @var int
 * @cvalue AI_V4MAPPED
 */
const AI_V4MAPPED = UNKNOWN;
#endif
#ifdef AI_ALL
/**
 * @var int
 * @cvalue AI_ALL
 */
const AI_ALL = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue AI_ADDRCONFIG
 */
const AI_ADDRCONFIG = UNKNOWN;
#ifdef AI_IDN
/**
 * @var int
 * @cvalue AI_IDN
 */
const AI_IDN = UNKNOWN;
/**
 * @var int
 * @cvalue AI_CANONIDN
 */
const AI_CANONIDN = UNKNOWN;
#endif
#ifdef AI_NUMERICSERV
/**
 * @var int
 * @cvalue AI_NUMERICSERV
 */
const AI_NUMERICSERV = UNKNOWN;
#endif
#ifdef SOL_LOCAL
/**
 * @var int
 * @cvalue SOL_LOCAL
 */
const SOL_LOCAL = UNKNOWN;
#endif

#if (defined(IPV6_RECVPKTINFO) && HAVE_IPV6)
/**
 * IPv6 ancillary data
 * @var int
 * @cvalue IPV6_RECVPKTINFO
 */
const IPV6_RECVPKTINFO = UNKNOWN;
/**
 * @var int
 * @cvalue IPV6_PKTINFO
 */
const IPV6_PKTINFO = UNKNOWN;
#endif
#if (defined(IPV6_RECVHOPLIMIT) && HAVE_IPV6)
/**
 * @var int
 * @cvalue IPV6_RECVHOPLIMIT
 */
const IPV6_RECVHOPLIMIT = UNKNOWN;
/**
 * @var int
 * @cvalue IPV6_HOPLIMIT
 */
const IPV6_HOPLIMIT = UNKNOWN;
#endif

#if (defined(IPV6_RECVTCLASS) && HAVE_IPV6)
/**
 * @var int
 * @cvalue IPV6_RECVTCLASS
 */
const IPV6_RECVTCLASS = UNKNOWN;
/**
 * @var int
 * @cvalue IPV6_TCLASS
 */
const IPV6_TCLASS = UNKNOWN;
#endif

#ifdef SCM_RIGHTS
/**
 * @var int
 * @cvalue SCM_RIGHTS
 */
const SCM_RIGHTS = UNKNOWN;
#endif
#ifdef SO_PASSCRED
#ifdef SCM_CREDENTIALS
/**
 * @var int
 * @cvalue SCM_CREDENTIALS
 */
const SCM_CREDENTIALS = UNKNOWN;
#else
/**
 * @var int
 * @cvalue SCM_CREDS
 */
const SCM_CREDS = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue SO_PASSCRED
 */
const SO_PASSCRED = UNKNOWN;
#endif
#if defined(LOCAL_CREDS_PERSISTENT)
/**
 * @var int
 * @cvalue SCM_CREDS2
 */
const SCM_CREDS2 = UNKNOWN;
/**
 * @var int
 * @cvalue LOCAL_CREDS_PERSISTENT
 */
const LOCAL_CREDS_PERSISTENT = UNKNOWN;
#endif
#if (!defined(LOCAL_CREDS_PERSISTENT) && defined(LOCAL_CREDS))
/**
 * @var int
 * @cvalue SCM_CREDS
 */
const SCM_CREDS = UNKNOWN;
/**
 * @var int
 * @cvalue LOCAL_CREDS
 */
const LOCAL_CREDS = UNKNOWN;
#endif
#if defined(SO_ATTACH_REUSEPORT_CBPF)
/**
 * @var int
 * @cvalue SO_ATTACH_REUSEPORT_CBPF
 */
const SO_ATTACH_REUSEPORT_CBPF = UNKNOWN;
#endif
#if defined(SO_DETACH_FILTER)
/**
 * @var int
 * @cvalue SO_DETACH_FILTER
 */
const SO_DETACH_FILTER = UNKNOWN;
#endif
#if defined(SO_DETACH_BPF)
/**
 * @var int
 * @cvalue SO_DETACH_BPF
 */
const SO_DETACH_BPF = UNKNOWN;
#endif
#if defined(SO_EXCLUSIVEADDRUSE)
/**
 * @var int
 * @cvalue SO_EXCLUSIVEADDRUSE
 */
const SO_EXCLUSIVEADDRUSE = UNKNOWN;
#endif
#if defined(TCP_QUICKACK)
/**
 * @var int
 * @cvalue TCP_QUICKACK
 */
const TCP_QUICKACK = UNKNOWN;
#endif
#if defined(TCP_REPAIR)
/**
 * @var int
 * @cvalue TCP_REPAIR
 */
const TCP_REPAIR = UNKNOWN;
#endif
#if defined(IP_DONTFRAG)
/**
 * @var int
 * @cvalue IP_DONTFRAG
 */
const IP_DONTFRAG = UNKNOWN;
#endif
#if defined(IP_MTU_DISCOVER)
/**
 * @var int
 * @cvalue IP_MTU_DISCOVER
 */
const IP_MTU_DISCOVER = UNKNOWN;
#endif
#if defined(IP_PMTUDISC_DO)
/**
 * @var int
 * @cvalue IP_PMTUDISC_DO
 */
const IP_PMTUDISC_DO = UNKNOWN;
#endif
#if defined(IP_PMTUDISC_DONT)
/**
 * @var int
 * @cvalue IP_PMTUDISC_DONT
 */
const IP_PMTUDISC_DONT = UNKNOWN;
#endif
#if defined(IP_PMTUDISC_WANT)
/**
 * @var int
 * @cvalue IP_PMTUDISC_WANT
 */
const IP_PMTUDISC_WANT = UNKNOWN;
#endif
#if defined(IP_PMTUDISC_PROBE)
/**
 * @var int
 * @cvalue IP_PMTUDISC_PROBE
 */
const IP_PMTUDISC_PROBE = UNKNOWN;
#endif
#if defined(IP_PMTUDISC_INTERFACE)
/**
 * @var int
 * @cvalue IP_PMTUDISC_INTERFACE
 */
const IP_PMTUDISC_INTERFACE = UNKNOWN;
#endif
#if defined(IP_PMTUDISC_OMIT)
/**
 * @var int
 * @cvalue IP_PMTUDISC_OMIT
 */
const IP_PMTUDISC_OMIT = UNKNOWN;
#endif
#if defined(UDPLITE_SEND_CSCOV)
/**
 * @var int
 * @cvalue UDPLITE_SEND_CSCOV
 */
const UDPLITE_SEND_CSCOV = UNKNOWN;
#endif
#if defined(UDPLITE_RECV_CSCOV)
/**
 * @var int
 * @cvalue UDPLITE_RECV_CSCOV
 */
const UDPLITE_RECV_CSCOV = UNKNOWN;
#endif

/**
 * @strict-properties
 * @not-serializable
 */
final class Socket
{
}

/**
 * @strict-properties
 * @not-serializable
 */
final class AddressInfo
{
}

function socket_select(?array &$read, ?array &$write, ?array &$except, ?int $seconds, int $microseconds = 0): int|false {}

function socket_create_listen(int $port, int $backlog = 128): Socket|false {}

function socket_accept(Socket $socket): Socket|false {}

function socket_set_nonblock(Socket $socket): bool {}

function socket_set_block(Socket $socket): bool {}

function socket_listen(Socket $socket, int $backlog = 0): bool {}

function socket_close(Socket $socket): void {}

function socket_write(Socket $socket, string $data, ?int $length = null): int|false {}

function socket_read(Socket $socket, int $length, int $mode = PHP_BINARY_READ): string|false {}

/**
 * @param string $address
 * @param int $port
 */
function socket_getsockname(Socket $socket, &$address, &$port = null): bool {}

/**
 * @param string $address
 * @param int $port
 */
function socket_getpeername(Socket $socket, &$address, &$port = null): bool {}

function socket_create(int $domain, int $type, int $protocol): Socket|false {}

function socket_connect(Socket $socket, string $address, ?int $port = null): bool {}

function socket_strerror(int $error_code): string {}

function socket_bind(Socket $socket, string $address, int $port = 0): bool {}

/** @param string|null $data */
function socket_recv(Socket $socket, &$data, int $length, int $flags): int|false {}

function socket_send(Socket $socket, string $data, int $length, int $flags): int|false {}

/**
 * @param string $data
 * @param string $address
 * @param int $port
 */
function socket_recvfrom(Socket $socket, &$data, int $length, int $flags, &$address, &$port = null): int|false {}

function socket_sendto(Socket $socket, string $data, int $length, int $flags, string $address, ?int $port = null): int|false {}

/**
 * @return array<string, mixed>|int|false
 * @refcount 1
 */
function socket_get_option(Socket $socket, int $level, int $option): array|int|false {}

/**
 * @return array<string, mixed>|int|false
 * @alias socket_get_option
 */
function socket_getopt(Socket $socket, int $level, int $option): array|int|false {}

/** @param array|string|int $value */
function socket_set_option(Socket $socket, int $level, int $option, $value): bool {}

/**
 * @param array|string|int $value
 * @alias socket_set_option
 */
function socket_setopt(Socket $socket, int $level, int $option, $value): bool {}

#ifdef HAVE_SOCKETPAIR
/** @param array $pair */
function socket_create_pair(int $domain, int $type, int $protocol, &$pair): bool {}
#endif

#ifdef HAVE_SHUTDOWN
function socket_shutdown(Socket $socket, int $mode = 2): bool {}
#endif

#ifdef HAVE_SOCKATMARK
function socket_atmark(Socket $socket): bool {}
#endif

function socket_last_error(?Socket $socket = null): int {}

function socket_clear_error(?Socket $socket = null): void {}

/** @param resource $stream */
function socket_import_stream($stream): Socket|false {}

/** @return resource|false */
function socket_export_stream(Socket $socket) {}

function socket_sendmsg(Socket $socket, array $message, int $flags = 0): int|false {}

function socket_recvmsg(Socket $socket, array &$message, int $flags = 0): int|false {}

function socket_cmsg_space(int $level, int $type, int $num = 0): ?int {}

/**
 * @return array<int, AddressInfo>|false
 * @refcount 1
 */
function socket_addrinfo_lookup(string $host, ?string $service = null, array $hints = []): array|false {}

function socket_addrinfo_connect(AddressInfo $address): Socket|false {}

function socket_addrinfo_bind(AddressInfo $address): Socket|false {}

/**
 * @return array<string, int|string|array>
 * @refcount 1
 */
function socket_addrinfo_explain(AddressInfo $address): array {}

#ifdef PHP_WIN32
function socket_wsaprotocol_info_export(Socket $socket, int $process_id): string|false {}

function socket_wsaprotocol_info_import(string $info_id): Socket|false {}

function socket_wsaprotocol_info_release(string $info_id): bool {}
#endif
