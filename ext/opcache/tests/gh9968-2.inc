<?php

class Root1_Constant_Definer
{
    const CONSTANT = 'value';
}

class Root1_Constant_Empty
{
}

class Root1_Constant_Referencer extends Root2_Empty_Empty
{
    protected $propertyReferencingAnExternalConstant = Root1_Constant_Definer::CONSTANT;
}
