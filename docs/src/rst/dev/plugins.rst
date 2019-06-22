Adding support for new video cards and drivers
==============================================

|project| does not support video cards by itself. This feature is added by :doc:`plugins </user/plugins>`. Each plugin is a set of files that will be loaded by the main application.


Plugin ID
---------

Each plugin must have an identifier that looks like this: ``<category>-<name>``.

* **Category** defines the type of supported devices. Example: ``amd``. Only one plugin from each category can be included at a time. Category name can consist of letters a-z in lower case and digits 0-9, and should start with a letter.

* **Name** sets the unique plugin identifier within the selected category. Example: ``mesa-free``. The name can consist of letters a-z in the lower case, digits 0-9 and a hyphen ``-``, and should start with a letter.

For the sake of brevity, the plugin identifier below will be referred to as **<id>**.


Plugin files
------------

The files of each plugin can be placed in one of the :doc:`search folders </user/resources>`, in the subfolder **plugins/<id>**. For example, if the plugin is called **amd-mesa-free** and one of the search folders /usr/local/share/|project-id|, the plugin can be placed in /usr/local/share/|project-id|/plugins/amd-mesa-free to be detected by the application. The plugin must contain at least three files:

* <id>-init
    The code in this file is executed before starting |project|. If the file finishes its work by returning a non-zero output code, the corresponding plugin will not be loaded and the user will receive a system message containing the text from the standard error output stream (stderr). The file can have any extension or none at all, but the operating system should be able to understand that the file is executable (for example, in Linux it is necessary to set an executable flag on this file).

    *Location example:* ``.../plugins/amd-mesa-free/amd-mesa-free-init.sh``

    This file can set new environment variables that will be passed to the main program and other plugins. To do this, the **<id>-init** file must output a string in the standard output stream (stdout) in the format of **<key>=<value>**. Empty lines, lines starting with ``#`` and lines without ```=`` are ignored. **<key>** cannot contain the symbol ``=``. If **<key>** or **<value>** contain spaces from the beginning or the end of a line, these spaces will be removed.

    Existing environment variables can be overwritten or appended to. If it is required to add new data in the end of an existing environment variable, the first symbol in a **<value>** line should be the operating system path delimiter (for example, for Linux it is ``:``). If it is required to add something in the beginning of a variable the delimiter should be the last symbol in a **<value>** line.

    *Example:*

    There are following existing environment variables:

    .. code-block:: text

        PATH=/bin:/usr/bin
        LD_LIBRARY_PATH=/usr/lib
        ABC=123
        BOGUS=simsalabim

    File **<id>-init** returns the following text in stdout:

    .. code-block:: text

        ABC = 666
        DEF = 7 7 7 7
        
        PATH=:/opt/bin:/home/user/bin
        #BOGUS=abracadabra
        LD_LIBRARY_PATH=/opt/lib:

    In this case, the final set of environment variables after the completion of **<id>-init** will be as follows:

    .. code-block:: text

        PATH=/bin:/usr/bin:/opt/bin:/home/user/bin
        LD_LIBRARY_PATH=/opt/lib:/usr/lib
        ABC=6666
        BOGUS=simsalabim
        DEF=7 7 7 7

    These environment variables will be accessed by all plugins and files loaded after this file, except for other **\*-init** files.


* <id>-deinit
    The code in this file is executed after |project| is closed. If the file returns a non-zero output code, the user will get a system message containing the text from the standard error output stream (stderr). This file is started only if **<id>-init** is successfully executed. The file will be executed even if a fatal failure that caused a sudden stop of the application has occurred in |project|. The file can have any extension or none at all, but the operating system should be able to understand that the file is executable (for example, in Linux it is necessary to set an executable flag on this file).

    *Location example:* ``.../plugins/amd-mesa-free/amd-mesa-free-deinit.py``


* gpufancontrol-<id>
    This file is a dynamic library that will be downloaded by |project| The file should have a name that corresponds to the standard name of the dynamic libraries in the operating system, including the major version number where applicable (e.g., for Linux the prefix "lib", the extension ".so" and the version number like ".1" should be added). The required library interface is described in the file `shared/include/gpufanmeister/plugin.h <https://github.com/alkatrazstudio/gpu-fan-meister/blob/master/shared/include/gpufanmeister/plugin.h>`_.

    *Location example:* ``.../plugins/amd-mesa-free/libgpufancontrol-amd-mesa-free.so.1``

    The library with the version number corresponding to the major version of |project| will be loaded. For example, if the version of |project| is **1.2.3**, then a library **libgpufancontrol-amd-mesa-free.so.1** will be loaded, and if the version of |project| is **2.3.4**, then a library **libgpufancontrol-amd-mesa-free.so.2** will be loaded. As a consequence, the plugin may contain several versions of the library. Libraries can be symbolic links.


Example
-------

As an example of a complete plugin you can take the sources of the :doc:`/user/plugins/nvidia-nvml` plugin, which comes with |project|: `<https://github.com/alkatrazstudio/gpu-fan-meister/blob/master/plugins/nvidia-nvml>`_.
