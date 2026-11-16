#!/usr/bin/env python3
"""Prepare NX809J local assets and source dependency patches; never flash a device."""
import argparse, hashlib, json, shutil, subprocess
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent

def git(repo, *args):
    return subprocess.run(['git', '-C', str(repo), *args], capture_output=True)

def patches():
    for folder, name in [('Common/Mu', 'mu-plus.patch'), ('Mu_Basecore', 'basecore.patch')]:
        data = (HERE / name).read_bytes()
        for block in data.split(b'diff --git ')[1:]:
            patch = b'diff --git ' + block
            # Apply individually so upstream fixes are recognized rather than reverted.
            forward = subprocess.run(['git','-C',str(ROOT/folder),'apply','--check','--ignore-space-change','-'],input=patch,capture_output=True)
            if forward.returncode == 0:
                subprocess.run(['git','-C',str(ROOT/folder),'apply','--ignore-space-change','-'],input=patch,check=True)
            else:
                reverse = subprocess.run(['git','-C',str(ROOT/folder),'apply','--reverse','--check','--ignore-space-change','-'],input=patch,capture_output=True)
                if reverse.returncode:
                    raise SystemExit('Dependency patch needs review: '+folder+'\n'+forward.stderr.decode(errors='replace'))

def assets(source):
    manifest = json.loads((HERE/'assets.json').read_text())
    missing = []
    for entry in manifest:
        dest = ROOT/'Binaries/nx809j'/entry['path']
        choices = []
        if source:
            choices.append(source/'Binaries/nx809j'/entry['path'])
        choices += [ROOT/'Binaries'/p for p in entry['upstream_matches']]
        if dest.exists(): choices.insert(0,dest)
        match = next((p for p in choices if p.is_file() and hashlib.sha256(p.read_bytes()).hexdigest()==entry['sha256']),None)
        if match is None:
            missing.append(entry['path']); continue
        dest.parent.mkdir(parents=True,exist_ok=True)
        if match.resolve()!=dest.resolve():shutil.copy2(match,dest)
    # These DTBs are supplied locally, not redistributed in this repository.
    for rel in ['Resources/DTBs/nx809j.dtb',
                'Platforms/ZTE/nx809jPkg/FdtBlob/kaanapali-nx809j.dtb',
                'Platforms/ZTE/nx809jPkg/FdtBlob/kaanapali-oneplus-nx809j.dtb']:
        dst=ROOT/rel
        if source and (source/rel).is_file():
            dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source/rel,dst)
        if not dst.is_file():missing.append(rel)
    if missing:raise SystemExit('Supply the original local asset tree with --from-port; missing assets:\n'+'\n'.join(missing))
    print('NX809J assets verified.')

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--from-port',type=Path,help='Existing local NX809J port containing recovered assets')
    parser.add_argument('--patches-only',action='store_true')
    args=parser.parse_args()
    if not args.patches_only:assets(args.from_port.resolve() if args.from_port else None)
    patches()
    print('NX809J source dependency patches ready.')
