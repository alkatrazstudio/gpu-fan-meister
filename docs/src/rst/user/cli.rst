Command line parameters
=======================

|project| supports the following command line parameters:

.. _cli-plugins:

* **--plugins** - allows you to specify a list of :doc:`plugins <plugins>`, which will be used to work with video cards. If the parameter is not set, the plugins that were last selected in the ":doc:`gui/enabledPlugins`" window will be used. The list of these plugins can be displayed through the :ref:`--enabled-plugins <cli-enabled-plugins>` parameter. If the parameter is omitted, and no plugins have been selected before, the ":doc:`gui/enabledPlugins`" window will appear.

.. _cli-available-plugins:

* **--available-plugins** - display the list of available installed :doc:`plugins <plugins>` one per line. It does not check whether these plugins can actually be loaded. For each plugin, its folder will also be displayed.

.. _cli-enabled-plugins:

* **--enabled-plugins** - display the list of :doc:`plugins <plugins>`, last selected in the ":doc:`gui/enabledPlugins`" window. In other words, these plugins will be loaded if you omit the :ref:`--plugins <cli-plugins>` parameter. For each plugin, its folder will also be displayed.

.. _cli-plugin-selector:

* **--plugin-selector** - show the ":doc:`gui/enabledPlugins`" window. After closing the window, the program will exit.

.. _cli-html-help:

* **--html-help** - start the browser and open the full application manual (you are reading now).

.. _cli-dirs:

* **--dirs** - get a list of all :doc:`search folders <resources>`.

.. _cli-load-qml:

* **--load-qml** - download all GUI files and then exit the program. This option is for internal use only.


.. _cli-version:

* **-v** or **--version** - show the name and version of the application.

.. _cli-help:

* **-h** or **--help** - show help on command line parameters.
