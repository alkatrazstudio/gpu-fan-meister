import subprocess
import os

this_dir = os.path.dirname(os.path.realpath(__file__))

app_name = "gpu-fan-meister"

ver_num = subprocess.check_output("../../common/modules/Version/version.py", stderr=subprocess.DEVNULL).decode()
project_version = "v{}".format(ver_num)

project = "GPU Fan Meister"
author = "Alkatraz Studio"
copyright = "2018, {}".format(author)
version = project_version
release = version

master_doc = "index"
html_theme = "sphinx_rtd_theme"
html_show_sourcelink = False
html_copy_source = False
todo_include_todos = True
smartquotes = False
html_logo = "../../assets/{}.png".format(app_name)
html_favicon = html_logo
locale_dirs = [os.path.join(this_dir, "locale")]
gettext_additional_targets = ["literal-block"]

html_context = {
    "css_files": ["_static/css/custom.css"]
}

extensions = [
    "sphinx.ext.imgmath",
    "sphinx.ext.todo"
]

imgmath_image_format = "svg"

defines = {
    "project": project,
    "project-id": app_name,
    "project-org": author
}
epilogLines = [".. |{}| replace:: {}".format(key, val) for key, val in defines.items()]
rst_epilog = "\n".join(epilogLines)
