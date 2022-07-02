--TEST--
Bug #52327 (base64_decode() improper handling of leading padding)
--FILE--
<?php
var_dump(
    base64_decode('=VGhl=ICc9=JyBz=eW1i=b2xz=IGFy=ZW4n=dCBh=bGxv=d2Vk=IHdo=ZXJl=IGkg=cHV0=IHRo=ZW0g=by5P'),
    base64_decode('=VGhl=ICc9=JyBz=eW1i=b2xz=IGFy=ZW4n=dCBh=bGxv=d2Vk=IHdo=ZXJl=IGkg=cHV0=IHRo=ZW0g=by5P', true)
);
?>
--EXPECT--
string(51) "The '=' symbols aren't allowed where i put them o.O"
bool(false)
