<?php

incluye __DIR__ . DIRECTORY_SEPARATOR . 'incluye.php';
incluye una vez __DIR__ . DIRECTORY_SEPARATOR . 'incluye.php';
evalúa("incluye __DIR__ . DIRECTORY_SEPARATOR . 'incluye.php';");
necesito __DIR__ . DIRECTORY_SEPARATOR . 'incluye.php';
necesito una vez __DIR__ . DIRECTORY_SEPARATOR . 'incluye.php';

$haceSol = true;
$tenemosVacaciones = true;
si ($haceSol y $tenemosVacaciones) {
    echo "vamos a la playa\n";
}
$tenemosVacaciones = false;
si ($haceSol o $tenemosVacaciones) {
    echo "vamos a la playa\n";
} si no, si ($haceSol o exclusivamente $tenemosVacaciones) {
    echo "vamos a la playa\n";
} si no {
    echo "no vamos a la playa\n";
}

function countTo4()
{
    entrega desde [1, 2, 3];
    entrega 4;
}

foreach (countTo4() as $number) {
    muestra($number);
    muestra("\n");
}

class Cosa {}

$cosa = construye Cosa();

si ($cosa es instancia de Cosa) {
    muestra('verdadero');
}

$clon = clona $cosa;
si ($clon es instancia de Cosa) {
    muestra('soy un clon de cosa');
    sal;
}