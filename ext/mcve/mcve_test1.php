<HTML>
<BODY>
<?php

// MCVE Config stuff
$username="vitale";
$password="test";
// 1:IP or 2:SSL
$method=2;
$host="testbox.mcve.com";
$port=8444;  //8444 is typically SSL and 8333 is standard
// End config stuff


if (!$account) 
  $account="4012888888881";
if (!$exp)
  $exp="0512";
if (!$amount)
  $amount=12.00;

function flush_buffer()
{
  for ($i=0; $i<2048; $i++) {
    echo "  ";
  }
  flush();
}


dl("./php_mcve.so");
  
  echo "Initializing MCVE<BR>";
  flush_buffer(); 
  mcve_initengine("./CAfile.pem");
  $conn=mcve_initconn();
  if ($method == 1)
    $ret=mcve_setip($conn, $host, $port);
  else if ($method == 2)
    $ret=mcve_setssl($conn, $host, $port);
  
  if (!$ret) {
    echo "Could not set method<BR>";
    exit(1);
  } 
  echo "Connection method and location set<BR>";
  flush_buffer();
  if (!mcve_connect($conn)) {
    echo "Connection Failed<BR>";
    exit(1); 
  }
  echo "Connection Established<BR>";
  flush_buffer();
  $identifier=mcve_sale($conn, $username, $password, NULL, $account, $exp,
              $amount, NULL, NULL, NULL, NULL, NULL, NULL, 001);
  echo "Transaction Sent: CC: $account EXP: $exp AMOUNT: $amount<BR>";
  flush_buffer();
  while (mcve_checkstatus($conn, $identifier) != MCVE_DONE) {
    mcve_monitor($conn);
  }
  echo "Transaction Complete<BR>";
  flush_buffer();
  $status=mcve_returnstatus($conn, $identifier);
  if ($status == MCVE_SUCCESS) {
    $text=mcve_transactiontext($conn, $identifier);
    $auth=mcve_transactionauth($conn, $identifier);
    echo "Transaction Authorized<BR>";
    echo "Auth: $auth<BR>";
    echo "Text: $text<BR>";
  }  else if ($status == MCVE_FAIL) {
    $text=mcve_transactiontext($conn, $identifier);
    echo "Transaction Denied<BR>";
    echo "Text: $text<BR>";
  } else 
    echo "Transaction error<BR>";
  flush_buffer();
  mcve_destroyconn($conn);
  mcve_destroyengine();

?>
</BODY>
</HTML>

