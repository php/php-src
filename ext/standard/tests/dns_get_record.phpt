--TEST--
array dns_get_record ( string $hostname [, int $type = DNS_ANY [, array &$authns [, array &$addtl [, bool &$raw = false ]]]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--FILE--
<?php
$hostname = "yahoo.com";

print((is_array(dns_get_record($hostname))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_A))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_CNAME))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_HINFO))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_MX))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_NS))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_PTR))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_SOA))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_TXT))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_AAAA))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_SRV))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_NAPTR))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_A6))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_ALL))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_ANY))) ? ("bool(true)\n") : ("bool(false)\n"));

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

print((is_array($authns_DNS_A)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_CNAME)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_HINFO)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_MX)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_NS)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_PTR)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_SOA)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_TXT)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_AAAA)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_SRV)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_NAPTR)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_A6)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_ALL)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($authns_DNS_ANY)) ? ("bool(true)\n") : ("bool(false)\n"));



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

print((is_array($addtl_DNS_A)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_CNAME)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_HINFO)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_MX)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_NS)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_PTR)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_SOA)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_TXT)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_AAAA)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_SRV)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_NAPTR)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_A6)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_ALL)) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array($addtl_DNS_ANY)) ? ("bool(true)\n") : ("bool(false)\n"));

print((is_array(dns_get_record($hostname, DNS_A, $authns_DNS_A, $addtl_DNS_A, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_CNAME, $authns_DNS_CNAME, $addtl_DNS_CNAME, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_HINFO, $authns_DNS_HINFO, $addtl_DNS_HINFO, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_MX, $authns_DNS_MX, $addtl_DNS_MX, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_NS, $authns_DNS_NS, $addtl_DNS_NS, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_PTR, $authns_DNS_PTR, $addtl_DNS_PTR, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_SOA, $authns_DNS_SOA, $addtl_DNS_SOA, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_TXT, $authns_DNS_TXT, $addtl_DNS_TXT, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_AAAA, $authns_DNS_AAAA, $addtl_DNS_AAAA, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_SRV, $authns_DNS_SRV, $addtl_DNS_SRV, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_NAPTR, $authns_DNS_NAPTR, $addtl_DNS_NAPTR, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_A6, $authns_DNS_A6, $addtl_DNS_A6, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_ALL, $authns_DNS_ALL, $addtl_DNS_ALL, false))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_ANY, $authns_DNS_ANY, $addtl_DNS_ANY, false))) ? ("bool(true)\n") : ("bool(false)\n"));

print((is_array(dns_get_record($hostname, DNS_A, $authns_DNS_A, $addtl_DNS_A, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_CNAME, $authns_DNS_CNAME, $addtl_DNS_CNAME, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_HINFO, $authns_DNS_HINFO, $addtl_DNS_HINFO, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_MX, $authns_DNS_MX, $addtl_DNS_MX, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_NS, $authns_DNS_NS, $addtl_DNS_NS, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_PTR, $authns_DNS_PTR, $addtl_DNS_PTR, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_SOA, $authns_DNS_SOA, $addtl_DNS_SOA, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_TXT, $authns_DNS_TXT, $addtl_DNS_TXT, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_AAAA, $authns_DNS_AAAA, $addtl_DNS_AAAA, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_SRV, $authns_DNS_SRV, $addtl_DNS_SRV, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_NAPTR, $authns_DNS_NAPTR, $addtl_DNS_NAPTR, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_A6, $authns_DNS_A6, $addtl_DNS_A6, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_ALL, $authns_DNS_ALL, $addtl_DNS_ALL, true))) ? ("bool(true)\n") : ("bool(false)\n"));
print((is_array(dns_get_record($hostname, DNS_ANY, $authns_DNS_ANY, $addtl_DNS_ANY, true))) ? ("bool(true)\n") : ("bool(false)\n"));

?>
--EXPECTF--
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)
bool(%s)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(%s)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(%s)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(%s)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(%s)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(%s)

Warning: dns_get_record(): Numeric DNS record type must be between %d and %d, '%s' given in %s on line %d
bool(%s)
