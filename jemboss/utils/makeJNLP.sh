#!/bin/sh


cd ..
if [ ! -d "jnlp" ]; then
  mkdir jnlp
fi

JAVA_HOME=0
while [ ! -f "$JAVA_HOME/bin/keytool" ]
do
  echo "Enter java (1.3 or above) location (/usr/java/jdk1.3.1/): "
  read JAVA_HOME
done

PATH=$PATH:$JAVA_HOME/bin/ ; export PATH

#
# Wrap client.keystore for JNLP 

if [ -f "resources/client.keystore" ]; then
  echo
  echo "Create client.jar to contain client.keystore."
  cd resources
  jar cf client.jar client.keystore
  cd ..
else
  echo
  echo "*** WARNING! If you are using an SSL Jemboss server then"
  echo "*** this will not work as the script has not found the"
  echo "*** client keystore file."
fi

#
# Create Jemboss jar file

jar cf Jemboss.jar images/* org/emboss/jemboss/*class resources/*.jar \
        resources/version resources/jemboss.properties resources/readme.txt \
        org/emboss/jemboss/*/*class org/emboss/jemboss/*/*/*class 
mv Jemboss.jar jnlp
cp lib/*jar jnlp
cp images/Jemboss_logo_large.gif jnlp
cp utils/template.html jnlp/index.html
cd jnlp

echo
echo
echo "The following information is used by keytool to"
echo "create a key store...."
echo
echo "What is your first and last name [Unknown]?"
read NAME
echo "What is the name of your organizational unit [Unknown]?"
read ORGU
echo "What is the name of your organization [Unknown]?"
read ORG
echo "What is the name of your City or Locality [Unknown]?"
read LOC
echo "What is the name of your State or Province [Unknown]?"
read STATE
echo "What is the two-letter country code for this unit [Unknown]?"
read CODE

echo "Give a key password (at least 6 characters):"
read KEYPASS
echo "Give a store password (at least 6 characters):"
read STOREPASS

#
# create a keystore file

keytool -genkey -alias signFiles -dname "CN=$NAME, \
        OU=$ORGU, O=$ORG, L=$LOC, S=$STATE, C=$CODE" \
        -keypass $KEYPASS -storepass $STOREPASS -keystore jembossstore 

#
# sign each of the jar files

echo
echo
echo "Each of the jar files will now be signed"
echo
for i in *.jar; do 
  echo "Signing $i"
  jarsigner -keystore jembossstore -signedjar s$i \
        $i signFiles -storepass $STOREPASS
done;

#
# create a jnlp template file

JNLP="Jemboss.jnlp"
if [ -f "$JNLP" ]; then
  echo "$JNLP exists. Enter a new JNLP file name: "
  read JNLP
fi

echo '<!-- JNLP File for SwingSet2 Demo Application -->' > $JNLP
echo '<jnlp'                                            >> $JNLP
echo '        spec="1.0+"'                              >> $JNLP
echo '        codebase="http://EDIT"'                   >> $JNLP
echo '        href="'$JNLP'">'                          >> $JNLP 
echo '         <information>'                           >> $JNLP  
echo '           <title>Jemboss</title>'                >> $JNLP  
echo '           <vendor>HGMP-RC</vendor> '             >> $JNLP  
echo '           <homepage href="http://www.uk.embnet.org/Software/EMBOSS/Jemboss/"/>' \
                                                        >> $JNLP  
echo '           <description>Jemboss</description>'    >> $JNLP  
echo '           <description kind="short">A Java user interface to EMBOSS.' \
                                                        >> $JNLP  
echo '           </description>'                        >> $JNLP 
echo '           <icon href="Jemboss_logo_large.gif"/>' >> $JNLP 
echo '           <offline-allowed/>'                    >> $JNLP 
echo '         </information>'                          >> $JNLP 
echo '         <security>'                              >> $JNLP 
echo '           <all-permissions/>'                    >> $JNLP 
echo '         </security>'                             >> $JNLP 
echo '         <resources>'                             >> $JNLP 
echo '           <j2se version="1.3+"/>'                >> $JNLP 

for i in s*.jar; do
  echo '             <jar href="'$i'"/>'                >> $JNLP
done;

echo '         </resources>'                            >> $JNLP
echo '         <application-desc main-class="org/emboss/jemboss/Jemboss"/>' \
                                                        >> $JNLP
echo '       </jnlp>'                                   >> $JNLP
 
#
#
 
echo
echo "Created the signed jar files, index.html and $JNLP"
echo "in the directory ../jnlp."
echo "Please edit the codebase line in $JNLP."
echo "Also, edit the 'Click here' line in index.html to point href at $JNLP."
echo

