External resources
==================

|project| can download various third-party resources: :doc:`plugins <plugins>`, documentation and localization. |project| searches for these resources in specific folders. The list of these folders depends on the operating system. A complete list of folders where |project| will search for resources can be obtained through the :ref:`--dirs <cli-dirs>` parameter of the command line. When trying to load a resource |project| searches in all search folders and tries to find the resource in a particular subfolder that matches the resource type. The process of searching and loading each resource type is described below.


Plugins
-------

The process of searching and loading :doc:`plugins <plugins>` is as follows:

1. All folders are searched in the order returned by the CLI parameter :ref:`--dirs <cli-dirs>`.

2. The plugin is searched in the folder **<current search folder>/plugins/<plugin identifier>**.

3. If a plugin is found, then this plugin is loaded, and any plugin with the same name will not be loaded from all other search folders.


**Example:** If you have a search folder /usr/local/share/|project-id| and a plugin called **amd-mesa**, |project| will be able to load this plugin from /usr/local/share/|project-id|/plugins/amd-mesa.


To create your own plugin for |project|, see ":doc:`/dev/plugins`". You can find an example plugin here: `<https://github.com/alkatrazstudio/gpu-fan-meister/blob/master/plugins>`_.


.. _resources-docs:

Documentation
-------------

The process of searching and loading a documentation is as follows:

1. All folders are searched in the order returned by the CLI parameter :ref:`--dirs <cli-dirs>`.

2. Look for the folder **<current search folder>/docs/<language code>**, where **<language code>** is IETF-code\ [#ietf]_ of the language of the system locale.

3. If a folder is found, the documentation from this folder is used. All other folders are ignored.

4. If no documentation is found in any folder to search for, the last element after the "-" sign is removed from **<language code>** (zh-TW => zh). The process is then repeated.

5. If the last element of **<language code>** is removed and the documentation is still not found, |project| searches the search folders for documentation for the "en" language (which is guaranteed to be present in at least one search folder).


**Example:** If you have a search folder /usr/local/share/|project-id| and French Canadian is your system locale, |project| will search the documentation in /usr/local/share/|project-id|/docs/fr-CA, /usr/local/share/|project-id|/docs/fr and /usr/local/share/|project-id|/docs/en.


You can translate the |project| documentation into any language. The documentation is written in `reStructuredText <http://docutils.sourceforge.net/rst.html>`_ and built via `Sphinx <https://www.sphinx-doc.org/>`_. The process of localization/internationalization in Sphinx is described here: `<https://www.sphinx-doc.org/en/master/usage/advanced/intl.html>`_. The script ``docs/src/build.sh`` already does a lot of job for you. In summary, you need to do the following:

1. Clone the main repository:

    ``git clone https://github.com/alkatrazstudio/gpu-fan-meister``

2. Generate or update PO-files:

    ``docs/src/build.sh <language IETF-code> po``

3. Translate the PO-files.

4. Run the documentation build script:

    ``docs/src/build.sh <language IETF-code>``

5. Move the generated documentation from the folder **docs/<language IETF-code>** to any :ref:`search folder<resources-docs>`.


Localization
------------

The process of searching and loading localization files is as follows:

1. All folders are searched in the order opposite to the one returned by the CLI parameter :ref:`--dirs <cli-dirs>`.

2. Localization files are searched in the **<current search folder>/translations** and loaded according to Qt rules: `<https://doc.qt.io/qt-5.12/qtranslator.html#load-1>`_.

3. If a localization file is found, the translation it contains is added to the already loaded localization files. If there are duplicates of translations, the priority will be given to the last loaded localization.


**Example:** If you have a search folder /usr/local/share/|project-id| and German is your system locale, |project| can search for files with translations in /usr/local/share/|project-id|/translations/de-DE.qm (or other de-\*.qm files depending on the locale) and /usr/local/share/|project-id|/translations/de.qm.

To create your own translation for |project|, see the Qt Linguist Help: `<https://doc.qt.io/qt-5.12/linguist-translators.html>`_.



.. rubric:: Notes

.. [#ietf] Wikipedia article "IETF language tag": `<https://en.wikipedia.org/wiki/IETF_language_tag>`_.
