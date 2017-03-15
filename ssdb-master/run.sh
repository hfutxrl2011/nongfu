find . -name "*.h" -print | xargs sed -i ":begin; { /\*\//! { $! { N; b begin }; }; s/\/\*.*\*\// /; };"
#find . -name "*.cpp" | xargs -i sed -i ":begin; { /\*\//! { $! { N; b begin }; }; s/\/\*.*\*\// /; };" 
#sed -i ":begin; { /\*\//! { $! { N; b begin }; }; s/\/\*.*\*\// /; };" *.cpp
#sed -i ":begin; { /\*\//! { $! { N; b begin }; }; s/\/\*.*\*\// /; };" *.h
