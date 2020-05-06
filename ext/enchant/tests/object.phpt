--TEST--
Object API
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_object(new EnchantBroker)) {die("skip, resource dont load\n");}
 ?>
--FILE--
<?php
echo "+ Broker\n";
var_dump($broker = new EnchantBroker);
var_dump(is_array($broker->describe()));
$dicts = $broker->listDicts();
var_dump($lang = $dicts[0]['lang_tag']);

echo "+ Dict\n";
var_dump($dict = $broker->requestDict($lang));
var_dump(is_array($dict->describe()));
unset($dict);

echo "+ Check\n";
var_dump($dict = new EnchantDict($broker, $lang));
$w = "ElePHPant";
var_dump($dict->check($w));
var_dump($dict->isAdded($w));
$dict->addToSession($w);
var_dump($dict->check($w));
var_dump($dict->isAdded($w));

echo "+ Suggest\n";
var_dump(is_array($dict->suggest("soong")));

echo "+ PWL\n";
var_dump($dict = new EnchantDict($broker, '', __DIR__ . '/enchant_broker_request_pwl_dict.pwl'));
var_dump($dict->check('php'));
var_dump($dict->check('node'));
?>
OK
--EXPECTF--
+ Broker
object(EnchantBroker)#%d (0) {
}
bool(true)
string(%d) "%s"
+ Dict
object(EnchantDict)#%d (0) {
}
bool(true)
+ Check
object(EnchantDict)#%d (0) {
}
bool(false)
bool(false)
bool(true)
bool(true)
+ Suggest
bool(true)
+ PWL
object(EnchantDict)#%d (0) {
}
bool(true)
bool(false)
OK
