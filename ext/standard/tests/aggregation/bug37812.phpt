--TEST--
Bug #37812 (aggregate_methods_by_list fails to take certain methods)
--FILE--
<?php
class Absorber
{
        /**
        *   Assigns object's properties from supplied array
        *   @param array associative
        */

    function absorb($data)
    {
        $props = get_object_vars($this);

        foreach (array_keys($props) as $prop)
        {
            if (isset($data[$prop]))
            {
                $this->$prop = $data[$prop];
            }
        }
    }
}

class User
{
    function User($id = NULL)
    {
        // doesn't work
        aggregate_methods_by_list($this, 'Absorber', array('absorb'));
        echo '<pre>Aggregation:'.print_r(aggregation_info($this),1).'</pre>';
    }
}
new User;
?>
--EXPECT--
<pre>Aggregation:Array
(
    [absorber] => Array
        (
            [methods] => Array
                (
                    [0] => absorb
                )

            [properties] => Array
                (
                )

        )

)
</pre>

