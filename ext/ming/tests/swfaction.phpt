--TEST--
Ming: Simple SWFAction() test
--SKIPIF--
<?php if (!extension_loaded("ming")) print "skip"; ?>
--FILE--
<?php /* $Id$ */

  $s = new SWFShape();
  $f = $s->addFill(0xff, 0, 0);
  $s->setRightFill($f);

  $s->movePenTo(-500,-500);
  $s->drawLineTo(500,-500);
  $s->drawLineTo(500,500);
  $s->drawLineTo(-500,500);
  $s->drawLineTo(-500,-500);

  $p = new SWFSprite();
  $i = $p->add($s);
  $i->setDepth(1);
  $p->nextFrame();

  for($n=0; $n<5; ++$n)
  {
    $i->rotate(-15);
    $p->nextFrame();
  }

  $m = new SWFMovie();
  $m->setBackground(0xff, 0xff, 0xff);
  $m->setDimension(6000,4000);

  $i = $m->add($p);
  $i->setDepth(1);
  $i->moveTo(1000,2000);
  $i->setName("box");

  $m->add(new SWFAction("_root.box._x += 3;"));
  $m->nextFrame();
  $m->add(new SWFAction("gotoFrame(0); play();"));
  $m->nextFrame();

  $m->save('./test.swf');
  echo md5_file('./test.swf'), "\n";
  unlink('./test.swf');
?>
--EXPECT--
9e47538692393b9915faf3fc7e686cd5
