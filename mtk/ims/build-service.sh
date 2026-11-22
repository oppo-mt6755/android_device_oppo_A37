#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/../../../../.."
ims_source=device/oppo/A37/mtk/ims/service
ims_work=$(mktemp -d)
trap 'rm -rf "$ims_work"' EXIT
mkdir -p "$ims_work/stubs" "$ims_work/classes" "$ims_work/dex"
ims_android=prebuilts/sdk/27/android.jar
javac -source 1.8 -target 1.8 -nowarn -d "$ims_work/stubs" -cp "$ims_android" $(find "$ims_source/ImsService/compat/stubs" -name '*.java')
javac -source 1.8 -target 1.8 -nowarn -d "$ims_work/classes" -cp "$ims_android:$ims_work/stubs" $(find "$ims_source/ImsService/compat/src" -name '*.java')
(cd "$ims_work/stubs" && jar cf "$ims_work/stubs.jar" .)
java -cp prebuilts/sdk/tools/lib/d8.jar com.android.tools.r8.D8 --min-sdk-version 23 --lib "$ims_android" --classpath "$ims_work/stubs.jar" --output "$ims_work/dex" $(find "$ims_work/classes" -name '*.class')
cp -R "$ims_source/ImsService/smali" "$ims_work/smali"
java -jar vendor/lineage/build/tools/smali/baksmali.jar d "$ims_work/dex/classes.dex" -o "$ims_work/compat-smali"
cp -R "$ims_work/compat-smali/." "$ims_work/smali/"
java -jar vendor/lineage/build/tools/smali/smali.jar a --api 23 "$ims_work/smali" -o "$ims_work/classes.dex"
cp "$ims_source/ImsService/ImsService-stock.apk" "$ims_work/service.apk"
zip -q -d "$ims_work/service.apk" 'META-INF/*'
(cd "$ims_work" && zip -q service.apk classes.dex)
prebuilts/sdk/tools/linux/bin/zipalign -f -p 4 "$ims_work/service.apk" "$ims_source/ImsService.apk"
