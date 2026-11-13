--TEST--
curl_reset() releases callbacks, CURLOPT_PRIVATE and file post fields
--EXTENSIONS--
curl
--FILE--
<?php
class Marker
{
    public function __construct(private string $name) {}

    public function __invoke(...$args): int
    {
        return 0;
    }

    public function __destruct()
    {
        echo $this->name, " destroyed\n";
    }
}

class File extends CURLStringFile
{
    public function __destruct()
    {
        echo "post file destroyed\n";
    }
}

$handle = curl_init();
curl_setopt($handle, CURLOPT_WRITEFUNCTION, new Marker('write callback'));
curl_setopt($handle, CURLOPT_HEADERFUNCTION, new Marker('header callback'));
curl_setopt($handle, CURLOPT_READFUNCTION, new Marker('read callback'));
curl_setopt($handle, CURLOPT_PRIVATE, new Marker('private value'));
curl_setopt($handle, CURLOPT_POSTFIELDS, ['file' => new File('data', 'file.txt')]);

curl_reset($handle);
echo "reset\n";
var_dump(curl_getinfo($handle, CURLINFO_PRIVATE));
?>
--EXPECT--
write callback destroyed
header callback destroyed
read callback destroyed
post file destroyed
private value destroyed
reset
bool(false)
