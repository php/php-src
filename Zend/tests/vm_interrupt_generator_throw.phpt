--TEST--
VM interrupt handled while Generator::throw() resumes the generator
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=0
zend_test.observer.observe_function_names=gen
zend_test.observer.set_vm_interrupt_on_begin=1
--FILE--
<?php
class Dtor {
    public function __destruct() {
        echo "destroyed\n";
    }
}

function gen() {
    try {
        yield new Dtor();
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
    echo "resumed\n";
    yield 2;
}

$g = gen();
var_dump($g->current() instanceof Dtor);
var_dump($g->throw(new Exception('thrown')));
unset($g);
echo "done\n";
?>
--EXPECT--
bool(true)
Exception: thrown
resumed
destroyed
int(2)
done
