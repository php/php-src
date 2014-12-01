--TEST--
Bug #67704 (CURLOPT_POSTFIELDS modify the type of param)
--SKIPIF--
<?php 
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
?>
--FILE--
<?php
$ch = curl_init();
$post_data = array('a' => 1, 'b' => '2'); 
curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
var_dump($post_data['a']);                                                                                                                                  
curl_close($ch);              

$ch = curl_init();            
$one = 1;                     
$post_data = array('a' => $one, 'b' => '2');
curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
var_dump($post_data['a']);                                                                                                                                  
curl_close($ch);

$ch = curl_init();
$one_with_ref = 1;
$post_data = array('a' => &$one_with_ref, 'b' => '2');
curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
var_dump($post_data['a']);                                                                                                                                  
curl_close($ch);

$ch = curl_init();            
$post_data = array('a' => true);
curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
var_dump($post_data['a']);                                                                                                                                                                                    
curl_close($ch);

?>
--EXPECTF--
int(1)
int(1)
int(1)
bool(true)
