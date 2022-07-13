<!-- <?php
  // $array = [];
  // for($i=1;$i<100;$i++){
  //   $array[] = $i;
  // }
  // print_r(array_slice($array,20,28));
  // print_r(array_slice($array,61,37));
    
?>
<?php
  // $array = [];
  // $array2 = [];
  // for($i=21;$i<49;$i++){
  //   $array[] = $i;
  // }
  // for($x=62;$x<100;$x++){
  //   $array2[] = $x;
  // }
  // print_r($array);
  // print_r($array2);
    
?> -->
<?php
 
?>
<!-- dimpak da homework project -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
</head>
<body>
<div class="container">
  <form action="" method="post">
     <input class="form-control mt-4" type="number" placeholder="input starting array number" name="input">
     <input class="form-control mt-4" type="number" placeholder="ending number" name="output">
     <button type="submit" class="btn btn-outline-primary mt-4" name="btn" value="btn" >Print</button>
  </form>
  <div class="container" style="background-color:lime;">
      <?php
        if(isset($_POST['btn'])){
          $x = $_POST['input'];
          $y = $_POST['output'];
          $array = array();
          for($i = $x; $i <= $y; $i++){
            $array= $i.'=>'.$i;
            print_r($array);
            echo'<input class="form-control mt-4" type="number" placeholder="Enlist Things"><br>'; 
          }   
          
        }
      
      ?>
  </div>
<script src="https://code.jquery.com/jquery-3.3.1.slim.min.js" integrity="sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo" crossorigin="anonymous"></script>
<script src="https://cdn.jsdelivr.net/npm/popper.js@1.14.7/dist/umd/popper.min.js" integrity="sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1" crossorigin="anonymous"></script>
<script src="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/js/bootstrap.min.js" integrity="sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM" crossorigin="anonymous"></script>
</body>
</html>
