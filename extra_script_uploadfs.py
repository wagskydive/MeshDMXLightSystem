Import("env")

import os

def before_upload(source, target, env):
    print("Uploading SPIFFS filesystem...")
    spiffs_dir = os.path.join(env['PROJECT_DIR'], 'data')
    if not os.path.isdir(spiffs_dir):
        print("SPIFFS data folder not found!")
        return

    env.Replace(
        UPLOADCMD=" ".join([
            env['PYTHONEXE'],
            env['PIOPACKAGEMGR'],
            'run',
            '-t',
            'uploadfs'
        ])
    )

env.AddPreAction("upload", before_upload)
