import subprocess

Import("env")

def get_commit_hash(short = False):
    args = ["git", "rev-parse", "HEAD"]
    if short:
        args.insert(2, "--short")

    ret = subprocess.run(args, stdout=subprocess.PIPE, text=True)
    return ret.stdout.strip()
    
def get_commit_hash_build_flag():
    commit_hash = get_commit_hash()
    build_flag = "-D TALOS_VERSION_GIT_HASH=\\\"" + commit_hash + "\\\""
    print("BUILD COMMIT HASH: " + commit_hash)
    return build_flag

def get_latest_tag():
    ret = subprocess.run(["git", "describe", "--tags", "--abbrev=0"], stdout=subprocess.PIPE, text=True)
    return ret.stdout.strip()

def get_latest_tag_build_flag():
    tag = get_latest_tag()
    build_flag = "-D TALOS_VERSION_GIT_TAG=\\\"" + tag + "\\\""
    print("BUILD COMMIT TAG: " + tag)
    return build_flag

def get_commit_hash_build_flag_short():
    short_hash = get_commit_hash(True)
    build_flag = "-D TALOS_VERSION_GIT_HASH_SHORT=\\\"" + short_hash + "\\\""
    print("BUILD COMMIT HASH: " + short_hash)
    return build_flag

env.Append(
    BUILD_FLAGS=[
        get_commit_hash_build_flag(),
        get_commit_hash_build_flag_short(),
        get_latest_tag_build_flag()]
)