# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import sys, os, subprocess, re, glob

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'COFFI'
version = re.sub(r'[\s\S]*COFFI_VERSION.*?([\d\.]+)[\s\S]*', r'\1', open('../coffi/coffi_version.hpp').read())
release = version
language = 'en'

open('../version.doxyfile', 'w').write('PROJECT_NUMBER = "%s"\n' % version)


# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = []

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

master_doc = 'index_'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['static']

def run_doxygen(app):
    '''Run the doxygen command'''
    try:
        run_mod(app)
        retcode = subprocess.call('doxygen Doxyfile.in', shell = True, cwd = '.')
        if retcode < 0:
            sys.stderr.write('doxygen terminated by signal %s' % (-retcode))
    except OSError as e:
        sys.stderr.write('doxygen execution failed: %s' % e)

def run_mod(app):
    '''Replace "replacebyfile" in the *.m_o_d files by the file contents'''
    for mod in glob.glob('*.m_o_d'):
        txt = open(mod).read()
        for include in re.findall(r'^[ \t]*replacebyfile[ \t]+.*$', txt, re.MULTILINE):
            include_file = re.sub(r'[ \t]*replacebyfile[ \t]+(.*?)[ \t]*', r'\1', include)
            repl = open(os.path.join(os.path.dirname(mod), include_file)).read()
            txt = txt.replace(include, repl)
        md = mod.replace('.m_o_d', '.md')
        headerline = '<!-- This file is generated from %s by the script conf.py, DO NOT MODIFY! -->' % mod.replace('\\', '/')
        txt = re.sub(r'^(\s*[\\\\@](?:main)?page .*$)', r'\1\n' + headerline, txt, flags = re.MULTILINE)
        open(md, 'w').write(txt)

def setup(app):
    # Add hook for building doxygen xml when needed
    app.connect('builder-inited', run_doxygen)
