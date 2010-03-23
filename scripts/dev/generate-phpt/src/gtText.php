<?php

/**
 * Get a text message
 *
 */
class gtText
{

  /**
   * Get the text message and return it
   *
   * @param string $name
   * @return string
   */
  public static function get($name) {
    $filename = dirname(__FILE__) . '/texts/' . $name . '.txt';

    if (!file_exists($filename)) {
      throw new LogicException('The text ' . $name . ' does not exist');
    }

    return file_get_contents($filename);
  }
}

?>