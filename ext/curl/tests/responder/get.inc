<?php
  $test = isset($_GET['test']) ? $_GET['test'] : null;
  switch($test) {
    case 'post':
      var_dump($_POST);
      break;
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
    case 'httpversion':
      echo $_SERVER['SERVER_PROTOCOL'];
      break;
    case 'cookie':
      echo $_COOKIE['foo'];
      break;
    case 'encoding':
      echo $_SERVER['HTTP_ACCEPT_ENCODING'];
      break;
    case 'contenttype':
      header('Content-Type: text/plain;charset=utf-8');
      break;
    case 'file':
      if (isset($_FILES['file'])) {
          echo $_FILES['file']['name'] . '|' . $_FILES['file']['type'] . '|' . $_FILES['file']['size'];
      }
      break;
    case 'method':
      echo $_SERVER['REQUEST_METHOD'];
      break;
    default:
      echo "Hello World!\n";
      echo "Hello World!";
      break;
  }
?>
