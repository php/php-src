--TEST--
Test all filters returned by filter_list()
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
precision=14
default_charset=UTF-8
--FILE--
<?php
include dirname(__FILE__) . '/033_run.inc';
?>
--EXPECT--
int                      1                                               123                                                         
boolean                  1                                                                                                           
float                    1                                               123                                                         
validate_regexp                                                                                   O'Henry                            
validate_domain     PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc<>()          O'Henry        하퍼    aa:bb:cc:dd:ee:ff
validate_url                               http://a.b.c                                                                              
validate_email              foo@bar.com                                                                                              
validate_ip                                                    1.2.3.4                                                               
validate_mac                                                                                                               aa:bb:cc:dd:ee:ff
string              PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc()            O&#39;Henry    하퍼    aa:bb:cc:dd:ee:ff
stripped            PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc()            O&#39;Henry    하퍼    aa:bb:cc:dd:ee:ff
encoded             PHP  1  foo%40bar.com  http%3A%2F%2Fa.b.c  1.2.3.4   123  123abc%3C%3E%28%29  O%27Henry      %ED%95%98%ED%8D%BCaa%3Abb%3Acc%3Add%3Aee%3Aff
special_chars       PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc&#60;&#62;()  O&#39;Henry    하퍼    aa:bb:cc:dd:ee:ff
full_special_chars  PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc&lt;&gt;()    O&#039;Henry   하퍼    aa:bb:cc:dd:ee:ff
unsafe_raw          PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc<>()          O'Henry        하퍼    aa:bb:cc:dd:ee:ff
email               PHP  1  foo@bar.com    httpa.b.c           1.2.3.4   123  123abc              O'Henry                  aabbccddeeff
url                 PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc<>()          O'Henry                  aa:bb:cc:dd:ee:ff
number_int               1                                     1234      123  123                                                    
number_float             1                                     1234      123  123                                                    
magic_quotes        PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc<>()          O\'Henry       하퍼    aa:bb:cc:dd:ee:ff
add_slashes         PHP  1  foo@bar.com    http://a.b.c        1.2.3.4   123  123abc<>()          O\'Henry       하퍼    aa:bb:cc:dd:ee:ff
callback            PHP  1  FOO@BAR.COM    HTTP://A.B.C        1.2.3.4   123  123ABC<>()          O'HENRY        하퍼    AA:BB:CC:DD:EE:FF
