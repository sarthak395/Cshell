# Shell commands
uname : to get system name
uname -n : to get nodename

# C commands
## 1) uname struct (uts) and uts.sysname to get system name and uts.nodename to get username
```
struct utsname uts;
    uname(&uts);
    printf("System name is %s\n",uts.sysname);
    printf("Username is %s\n",uts.nodename);
```

## 2) coloring the output
```
033[1;31m Sample Text \033[0m
```
## 3) taking input with spaces
use "%[^\n]%*c" inside scanf