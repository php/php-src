<?php

  MCVE_InitEngine(NULL);
  $conn=MCVE_InitConn();
  if (MCVE_SetIP($conn, "testbox.mcve.com", 8333)) {
    echo "Set connection Properly";
  } else {
    echo "Failed Setting method";
    exit();
  }
  if (!MCVE_Connect($conn)) {
    echo "<BR>Could not connect<BR>";
  } else {
    echo "<BR>Connection Established<BR>";
  }
  MCVE_DestroyConn($conn);
  MCVE_DestroyEngine();

?>
