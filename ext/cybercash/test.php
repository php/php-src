<?php
 require "cyberlib.php";

 $merchant=""; /* Your merchant ID goes here. */
 $merchant_key=""; /* Your merchant key goes here. */
 $payment_url="http://cr.cybercash.com/cgi-bin/";
 $auth_type="mauthonly";
 
 $response=SendCC2_1Server($merchant,$merchant_key,$payment_url,
                $auth_type,array("Order-ID" => "2342322",
                "Amount" => "usd 11.50",
                "Card-Number" => "4111111111111111",
                "Card-Address" => "1600 Pennsylvania Avenue",
                "Card-City" => "Washington",
                "Card-State" => "DC",
                "Card-Zip" => "20500",
                "Card-Country" => "USA",
                "Card-Exp" => "12/99",
                "Card-Name" => "Bill Clinton"));

  while(list($key,$val)=each($response))
  {
    echo $key."=".$val."<br>";
  }

?>
