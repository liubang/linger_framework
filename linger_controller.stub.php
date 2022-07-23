<?php

/** @generate-legacy-arginfo */
abstract class Linger_Framework_Controller
{
    protected object $_request;
    protected object $_response;
    protected object $_view;

    public function __construct();

    protected function _init(): void;

    protected function getRequest(): object;

    protected function getResponse(): object;

    protected function getView(): object;
}
