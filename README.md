### Preview Output
![](coutput.png)
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
<hr>
### Features

- Support Standard Markdown / CommonMark and GFM(GitHub Flavored Markdown);
- Full-featured: Real-time Preview, Image (cross-domain) upload, Preformatted text/Code blocks/Tables insert, Code fold, Search replace, Read only, Themes, Multi-languages, L18n, HTML entities, Code syntax highlighting...;
- Markdown Extras : Support ToC (Table of Contents), Emoji, Task lists, @Links...;
- Compatible with all major browsers (IE8+), compatible Zepto.js and iPad;
- Support identification, interpretation, fliter of the HTML tags;
- Support TeX (LaTeX expressions, Based on KaTeX), Flowchart and Sequence Diagram of Markdown extended syntax;
- Support AMD/CMD (Require.js & Sea.js) Module Loader, and Custom/define editor plugins;

# About this project

![](https://github.com/pandao/editor.md/blob/master/images/loading@3x.gif?raw=true)

![](https://img.shields.io/github/stars/joshdejeu/CSC360-Project-4.svg) ![](https://img.shields.io/github/forks/joshdejeu/CSC360-Project-4.svg) ![](https://img.shields.io/github/tag/joshdejeu/CSC360-Project-4.svg) ![](https://img.shields.io/github/release/joshdejeu/CSC360-Project-4.svg) ![](https://img.shields.io/github/issues/joshdejeu/CSC360-Project-4.svg) ![](https://img.shields.io/bower/v/editor.md.svg)


**Table of Contents**

#### The Buffer struct

```h
#define BUFFER_SIZE 5
typedef struct
{
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    sem_t full;
    sem_t empty;
    pthread_mutex_t mutex;
} Buffer;
```

#### C Pthreads

```c
#include "buffer.h"
#include <pthread.h>
#include <semaphore.h>
#define number_of_producers 1
#define number_of_consumers 1
int main(int argc, char *argv[])
{
    Buffer myBuffer;
    buffer_initialize(&myBuffer); // semaphores + mutex

    pthread_t tid[number_of_producers];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_create(&tid[i], &attr, producer, (void *)&myBuffer);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_create(&tid[i], &attr, consumer, (void *)&myBuffer);
    }
    for (int i = 0; i < number_of_producers; i++)
    {
        pthread_join(tid[i], NULL);
    }
    for (int i = 0; i < number_of_consumers; i++)
    {
        pthread_join(tid[i], NULL);
    }
}
```


Random math idk looks cool。

$$\(\sqrt{3x-1}+(1+x)^2\)$$
                    
$$\sin(\alpha)^{\theta}=\sum_{i=0}^{n}(x^i + \cos(f))$$
                
### End
