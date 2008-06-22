<?php
  switch($_GET['test']) {
    case 'getpost':
      var_dump($_GET);
      var_dump($_POST);
      break;
    case 'referer':
      echo $_SERVER['HTTP_REFERER'];
      break;
    case 'useragent':
      echo $_SERVER['HTTP_USER_AGENT'];
      break;
    default:
      echo "Hello World!\n";
      echo "Hello World!";
      break;
  }
?>