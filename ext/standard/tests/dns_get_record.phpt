--TEST--
array dns_get_record ( string $hostname [, int $type = DNS_ANY [, array &$authns [, array &$addtl [, bool &$raw = false ]]]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) { die('skip: online test'); }
?>
--FILE--
<?php
$hostname = "php.net";

var_dump(is_array(dns_get_record($hostname)));
var_dump(is_array(dns_get_record($hostname, DNS_A)));
var_dump(is_array(dns_get_record($hostname, DNS_CNAME)));
var_dump(is_array(dns_get_record($hostname, DNS_HINFO)));
var_dump(is_array(dns_get_record($hostname, DNS_MX)));
var_dump(is_array(dns_get_record($hostname, DNS_NS)));
var_dump(is_array(dns_get_record($hostname, DNS_PTR)));
var_dump(is_array(dns_get_record($hostname, DNS_SOA)));
var_dump(is_array(dns_get_record($hostname, DNS_TXT)));
var_dump(is_array(dns_get_record($hostname, DNS_AAAA)));
var_dump(is_array(dns_get_record($hostname, DNS_SRV)));
var_dump(is_array(dns_get_record($hostname, DNS_NAPTR)));
var_dump(is_array(dns_get_record($hostname, DNS_A6)));
var_dump(is_array(dns_get_record($hostname, DNS_ALL)));
var_dump(is_array(dns_get_record($hostname, DNS_ANY)));

dns_get_record($hostname, DNS_A, $authns_DNS_A);
dns_get_record($hostname, DNS_CNAME, $authns_DNS_CNAME);
dns_get_record($hostname, DNS_HINFO, $authns_DNS_HINFO);
dns_get_record($hostname, DNS_MX, $authns_DNS_MX);
dns_get_record($hostname, DNS_NS, $authns_DNS_NS);
dns_get_record($hostname, DNS_PTR, $authns_DNS_PTR);
dns_get_record($hostname, DNS_SOA, $authns_DNS_SOA);
dns_get_record($hostname, DNS_TXT, $authns_DNS_TXT);
dns_get_record($hostname, DNS_AAAA, $authns_DNS_AAAA);
dns_get_record($hostname, DNS_SRV, $authns_DNS_SRV);
dns_get_record($hostname, DNS_NAPTR, $authns_DNS_NAPTR);
dns_get_record($hostname, DNS_A6, $authns_DNS_A6);
dns_get_record($hostname, DNS_ALL, $authns_DNS_ALL);
dns_get_record($hostname, DNS_ANY, $authns_DNS_ANY);

var_dump(is_array($authns_DNS_A));
var_dump(is_array($authns_DNS_CNAME));
var_dump(is_array($authns_DNS_HINFO));
var_dump(is_array($authns_DNS_MX));
var_dump(is_array($authns_DNS_NS));
var_dump(is_array($authns_DNS_PTR));
var_dump(is_array($authns_DNS_SOA));
var_dump(is_array($authns_DNS_TXT));
var_dump(is_array($authns_DNS_AAAA));
var_dump(is_array($authns_DNS_SRV));
var_dump(is_array($authns_DNS_NAPTR));
var_dump(is_array($authns_DNS_A6));
var_dump(is_array($authns_DNS_ALL));
var_dump(is_array($authns_DNS_ANY));

dns_get_record($hostname, DNS_A, $authns_DNS_A, $addtl_DNS_A);
dns_get_record($hostname, DNS_CNAME, $authns_DNS_CNAME, $addtl_DNS_CNAME);
dns_get_record($hostname, DNS_HINFO, $authns_DNS_HINFO, $addtl_DNS_HINFO);
dns_get_record($hostname, DNS_MX, $authns_DNS_MX, $addtl_DNS_MX);
dns_get_record($hostname, DNS_NS, $authns_DNS_NS, $addtl_DNS_NS);
dns_get_record($hostname, DNS_PTR, $authns_DNS_PTR, $addtl_DNS_PTR);
dns_get_record($hostname, DNS_SOA, $authns_DNS_SOA, $addtl_DNS_SOA);
dns_get_record($hostname, DNS_TXT, $authns_DNS_TXT, $addtl_DNS_TXT);
dns_get_record($hostname, DNS_AAAA, $authns_DNS_AAAA, $addtl_DNS_AAAA);
dns_get_record($hostname, DNS_SRV, $authns_DNS_SRV, $addtl_DNS_SRV);
dns_get_record($hostname, DNS_NAPTR, $authns_DNS_NAPTR, $addtl_DNS_NAPTR);
dns_get_record($hostname, DNS_A6, $authns_DNS_A6, $addtl_DNS_A6);
dns_get_record($hostname, DNS_ALL, $authns_DNS_ALL, $addtl_DNS_ALL);
dns_get_record($hostname, DNS_ANY, $authns_DNS_ANY, $addtl_DNS_ANY);

var_dump(is_array($addtl_DNS_A));
var_dump(is_array($addtl_DNS_CNAME));
var_dump(is_array($addtl_DNS_HINFO));
var_dump(is_array($addtl_DNS_MX));
var_dump(is_array($addtl_DNS_NS));
var_dump(is_array($addtl_DNS_PTR));
var_dump(is_array($addtl_DNS_SOA));
var_dump(is_array($addtl_DNS_TXT));
var_dump(is_array($addtl_DNS_AAAA));
var_dump(is_array($addtl_DNS_SRV));
var_dump(is_array($addtl_DNS_NAPTR));
var_dump(is_array($addtl_DNS_A6));
var_dump(is_array($addtl_DNS_ALL));
var_dump(is_array($addtl_DNS_ANY));

var_dump(is_array(dns_get_record($hostname, DNS_A, $authns_DNS_A, $addtl_DNS_A, false)));
var_dump(is_array(dns_get_record($hostname, DNS_CNAME, $authns_DNS_CNAME, $addtl_DNS_CNAME, false)));
var_dump(is_array(dns_get_record($hostname, DNS_HINFO, $authns_DNS_HINFO, $addtl_DNS_HINFO, false)));
var_dump(is_array(dns_get_record($hostname, DNS_MX, $authns_DNS_MX, $addtl_DNS_MX, false)));
var_dump(is_array(dns_get_record($hostname, DNS_NS, $authns_DNS_NS, $addtl_DNS_NS, false)));
var_dump(is_array(dns_get_record($hostname, DNS_PTR, $authns_DNS_PTR, $addtl_DNS_PTR, false)));
var_dump(is_array(dns_get_record($hostname, DNS_SOA, $authns_DNS_SOA, $addtl_DNS_SOA, false)));
var_dump(is_array(dns_get_record($hostname, DNS_TXT, $authns_DNS_TXT, $addtl_DNS_TXT, false)));
var_dump(is_array(dns_get_record($hostname, DNS_AAAA, $authns_DNS_AAAA, $addtl_DNS_AAAA, false)));
var_dump(is_array(dns_get_record($hostname, DNS_SRV, $authns_DNS_SRV, $addtl_DNS_SRV, false)));
var_dump(is_array(dns_get_record($hostname, DNS_NAPTR, $authns_DNS_NAPTR, $addtl_DNS_NAPTR, false)));
var_dump(is_array(dns_get_record($hostname, DNS_A6, $authns_DNS_A6, $addtl_DNS_A6, false)));
var_dump(is_array(dns_get_record($hostname, DNS_ALL, $authns_DNS_ALL, $addtl_DNS_ALL, false)));
var_dump(is_array(dns_get_record($hostname, DNS_ANY, $authns_DNS_ANY, $addtl_DNS_ANY, false)));

var_dump(is_array(dns_get_record($hostname, DNS_A, $authns_DNS_A, $addtl_DNS_A, true)));
var_dump(is_array(dns_get_record($hostname, DNS_CNAME, $authns_DNS_CNAME, $addtl_DNS_CNAME, true)));
var_dump(is_array(dns_get_record($hostname, DNS_HINFO, $authns_DNS_HINFO, $addtl_DNS_HINFO, true)));
var_dump(is_array(dns_get_record($hostname, DNS_MX, $authns_DNS_MX, $addtl_DNS_MX, true)));
var_dump(is_array(dns_get_record($hostname, DNS_NS, $authns_DNS_NS, $addtl_DNS_NS, true)));
var_dump(is_array(dns_get_record($hostname, DNS_PTR, $authns_DNS_PTR, $addtl_DNS_PTR, true)));
var_dump(is_array(dns_get_record($hostname, DNS_SOA, $authns_DNS_SOA, $addtl_DNS_SOA, true)));
var_dump(is_array(dns_get_record($hostname, DNS_TXT, $authns_DNS_TXT, $addtl_DNS_TXT, true)));
var_dump(is_array(dns_get_record($hostname, DNS_AAAA, $authns_DNS_AAAA, $addtl_DNS_AAAA, true)));
var_dump(is_array(dns_get_record($hostname, DNS_SRV, $authns_DNS_SRV, $addtl_DNS_SRV, true)));
var_dump(is_array(dns_get_record($hostname, DNS_NAPTR, $authns_DNS_NAPTR, $addtl_DNS_NAPTR, true)));
var_dump(is_array(dns_get_record($hostname, DNS_A6, $authns_DNS_A6, $addtl_DNS_A6, true)));
var_dump(is_array(dns_get_record($hostname, DNS_ALL, $authns_DNS_ALL, $addtl_DNS_ALL, true)));
var_dump(is_array(dns_get_record($hostname, DNS_ANY, $authns_DNS_ANY, $addtl_DNS_ANY, true)));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(false)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(false)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(false)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(false)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(false)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(false)
