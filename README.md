# php_linger_framework_extension

这是一个用c语言编写的PHP框架

**说明：**

目前还不稳定，不要用于生产环境，本人在php5.6环境中开发，对于php5.6以下版本和php7的兼容将会在后续处理。

## 目录结构

```
.
├── app
│   └── module
│       └── index
│           ├── controller
│           │   └── Index.php
│           └── view
│               ├── header.html
│               └── index.html
└── public
    └── index.php
```


## 示例代码

**public/index.php**

```php
<?php
session_start();
define('APP_PATH', realpath(__DIR__ . '/../') . '/');

set_exception_handler(function(Exception $e) {
	echo $e->getMessage(),PHP_EOL;
	echo $e->getTraceAsString();

});

$app = new linger\framework\Application([
	'app_directory' => APP_PATH . 'app',
]);
$app->run();
```

**app/module/index/controller/Index.php**

```php
<?php

class IndexController extends linger\framework\Controller {

    public function indexAction() {
        $list = [];
        for ($i = 0; $i < 10; ++$i) {
            $list[] = "这是测试{$i}";
        }
        $this->getView()
            ->assign('name', 'liubang')
            ->assign('list', $list)
            ->display('index.html');
    }
}
```


**app/module/index/view/index.html**

```html
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>
</head>
<body>
    <?=$this->render('header.html');?>
    <h1>hello <?=$name?></h1>
    <ul>
        <?php foreach($list as $value): ?>
            <li><?=$value?></li>
        <?php endforeach; ?>
    </ul>
</body>
</html>
```

**app/module/index/view/header.html**

```html
<header style="height: 75px; background: #ccc; text-align: center; line-height: 75px;">
    <h1>hello world</h1>
</header>
```

运行效果：

![](snapshot/1.png)

