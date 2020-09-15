import PyInstaller.__main__
import os
package_name = "Programmer"

PyInstaller.__main__.run([
    '--name=%s' % package_name,
    '--onefile',
    #'--icon="NOICON"',
    'Programmer.py'
])