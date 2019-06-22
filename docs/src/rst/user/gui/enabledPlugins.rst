Enabled plugins
===============

In this window you can choose :doc:`plugins </user/plugins>` that will be automatically loaded the next time you start the application.

The window shows all installed plugins, regardless of whether they can actually be loaded into |project| or not. If a plugin cannot be loaded when you start the program, you will receive a system message with details.

You need to select at least one plugin to run the program. Some plugins can be mutually exclusive and cannot be loaded in parallel. For example, two different plugins for video cards of the same developer may conflict with each other.


You can open this window by doing one of the following:

* Via :ref:`main menu <gui-mainWindow-mainMenu>`: **Application** => **Select plugins...**.

* Running the application via a :doc:`command line </user/cli>` with the :ref:`--plugin-selector <cli-plugin-selector>` parameter.


The window will open automatically in the following cases:

* At the application startup, but only if no plugins were specified via the :ref:`--plugins <cli-plugins>` command line parameter.

* If the last remaining loaded plugin crashes while the application is running.
