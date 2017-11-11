#This will check whether all required python modules are installed. If not it will give error. Please install appropriate python modules.

def install_and_import(package):
    import importlib
    try:
        importlib.import_module(package)
    except ImportError:
        import pip
        pip.main(['install', package])
    finally:
        globals()[package] = importlib.import_module(package)


install_and_import('pandas')
install_and_import('numpy')
install_and_import('pickle')
install_and_import('plotly')
install_and_import('sklearn')
install_and_import('matplotlib')
