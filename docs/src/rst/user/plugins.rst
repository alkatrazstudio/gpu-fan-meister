Plugins
=======

|project| does not support video cards by itself. This feature is added by plugins. Each plugin is a set of files that will be loaded by the main application.

Before you start using |project|, you need to install a plugin that supports your video card. You can download the required plugin from the Internet or :doc:`write it yourself </dev/plugins>`. Plugins should be placed in one of the :doc:`search folders <resources>`, in the subfolder **plugins/<id>**. For example, if the plugin is called **amd-mesa-free** and one of the search folders /usr/local/share/|project-id|, the plugin can be placed in /usr/local/share/|project-id|/plugins/amd-mesa-free to be detected by the application.

List of plugins supported by the |project| developer:

.. toctree::
    :maxdepth: 1
    :hidden:

    plugins/nvidia-nvml
    plugins/test-manual

* :doc:`plugins/nvidia-nvml` is a plugin for managing NVIDIA video cards that run under official proprietary drivers.
* :doc:`plugins/test-manual` - a plugin that emulates the video card and allows you to manually set the current temperature.

.. warning:: Downloaded plugins will have the same access rights as the account from which |project| is launched, i.e. almost full access to all your data.

The first time you start the program, the ":doc:`gui/enabledPlugins`" window will open, where you can select the plugins you want to automatically load at subsequent program launches. This window can also be opened manually - read more in :doc:`this window description <gui/enabledPlugins>`.
