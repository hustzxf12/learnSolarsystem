// OpenGL 里的光照效果
/**
 * 基本概念
 *
 * OpenGL 在处理光照时将光照系统分为了三个部分：光源、材质、光照环境。
 * 顾名思义，光源就是光的来源，比如太阳；
 * 材质这是指接受光照的各种物体的表面，比如太阳系中除太阳意外的行星和卫星都是这里所指的材质；
 * 光照环境则是一些额外的参数，他们讲应将最终得到的光照面积，比如光线通常会经过多次反射，这时制定一个『环境亮度』的参数，可以使得最后的画面接近真实情况。
 *
 * 物理学里，平行光射入光滑平面上后所得的反射光依然是平行光，这种反射被叫做『镜面反射』；
 * 而对于不光滑的平面造成的反射，便就是所谓的『漫反射』。
 */

/**
 * 光源
 * 在 OpenGL 里要实现光照系统，我们首先需要做的就是设置光源。
 * 值得一提的是，OpenGL 内只支持有限数量的光源（八个）分别使用 GL_LIGHT0 至 GL_LIGHT7 这八个宏来表示。
 * 通过 glEnable 函数来启用， glDisable 函数来禁用。例如：glEnable(GL_LIGHT0);
 * 设置光源位置则需要使用 glMaterialfv进行设置，例如：
 *
 * GLfloat light_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
 * glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 指定零号光源的位置
 *
 * 这里的位置由四个值来表示，(x,y,z,w) 其中当 w为0时，表示该光源位于无限远，而 x,y,z 便指定了这个无限远光源的方向；
 * 当 w 不为0时，表示 位置性光源，其位置为 (x/w, y/w, z/w) 。
 */

/**
 * 材质
 * 设置一个物体的材质一般有五个属性需要设置：
 *     1. 多次反射后追踪在环境中遗留的光照强度;
 *     2. 漫反射后的光照强度;
 *     3. 镜面反射后的光照强度;
 *     4. OpenGL 中不发光物体发出的微弱且不影像其他物体的光照强度;
 *     5. 镜面指数，指越小，表示材质越粗糙，点光源发射的光线照射后，产生较大的亮点；相反，值越大，材质越像镜面，产生较小的亮点。
 *
 * 设置材质 OpenGL 提供了两个版本的函数：
 *
 * void glMaterialf(GLenum face, GLenum pname, TYPE param);
 * void glMaterialfv(GLenum face, GLenum pname, TYPE *param);
 * 其差异在于，镜面指数只需要设置一个数值，这时只需要使用 glMaterialf；
 * 而其他的材质设置都需要设置多个值，这是需要使用数组进行设置，使用带指针向量参数的版本 glMaterialfv，例如：
 *
 * GLfloat mat_ambient[]  = {0.0f, 0.0f, 0.5f, 1.0f};
 * GLfloat mat_diffuse[]  = {0.0f, 0.0f, 0.5f, 1.0f};
 * GLfloat mat_specular[] = {0.0f, 0.0f, 1.0f, 1.0f};
 * GLfloat mat_emission[] = {0.5f, 0.5f, 0.5f, 0.5f};
 * GLfloat mat_shininess  = 90.0f;
 * glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
 * glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
 * glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
 * glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
 * glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);
 */

/**
 * 光照环境
 * OpenGL 是默认关闭光照处理的，要打开光照处理功能，需要使用 GL_LIGHTING 宏来激活，即 glEnable(GL_LIGHTING);。
 */

#include <cmath>
#include "star.hpp"

#define PI 3.1415926535

Star::Star(GLfloat radius, GLfloat distance,
           GLfloat speed, GLfloat selfSpeed, Star *parent):
radius(radius), selfSpeed(selfSpeed), distance(distance), parentStar(parent),
alpha(0), alphaSelf(0)
{
    for (int i = 0; i < 4; ++i)
        this->rgbaColor[i] = 1.0f;

    if (speed > 0)
        this->speed = 360.0f / speed;
    else
        this->speed = 0.0f;
}

void Star::drawStar()
{
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);

    int n = 1440;

    //保存 OpenGL 当前的矩阵环境
    glPushMatrix();
    {
        // 公转

        // 如果是行星, 切距离部位0, 那么想远点平移一个半径
        // 这部分用于处理卫星
        if (parentStar != 0 && parentStar->distance > 0)
        {
            glRotatef(parentStar->alpha, 0, 0, 1);

            glTranslatef(parentStar->distance, 0.0, 0.0);
        }

        // 绘制运行轨道
        glBegin(GL_LINES);
        for (int i = 0; i < n; ++i)
            glVertex2f(distance * cos(2 * PI * i / n),
                       distance * sin(2 * PI * i / n));
        glEnd();

        // 绕 z 轴旋转 alpha
        glRotated(alpha, 0, 0, 1);

        // x 轴方向平移 distance, y, z 方向不变
        glTranslatef(distance, 0.0, 0.0);

        // 自转
        glRotatef(alphaSelf, 0, 0, 1);

        // 绘制行星颜色
        glColor3f(rgbaColor[0], rgbaColor[1], rgbaColor[2]);

        glutSolidSphere(radius, 40, 32);
    }

    // 恢复绘制前的矩阵环境
    glPopMatrix();
}

void Star::update(long timeSpan)
{
    alpha += timeSpan * speed;  // 更新角度
    alphaSelf += selfSpeed;         // 更新自转角度
}

Planet::Planet(GLfloat radius, GLfloat distance,
               GLfloat speed, GLfloat selfSpeed,
               Star * parent, GLfloat rgbClolor[3]) :
    Star(radius, distance, speed, selfSpeed, parent)
{
    rgbaColor[0] = rgbClolor[0];
    rgbaColor[1] = rgbClolor[1];
    rgbaColor[2] = rgbClolor[2];
    rgbaColor[3] = 1.0f;
}




/**
 * 对于 Planet 类而言，属于不发光的星球，我们要绘制它的光照效果
 */
void Planet::drawPlanet()
{
    GLfloat mat_ambient[] = {0.0f, 0.0f, 0.5f, 1.0f};
    GLfloat mat_diffuse[] = {0.0f, 0.0f, 0.5f, 1.0f};
    GLfloat mat_specular[] = {0.0f, 0.0f, 1.0f, 1.0f};
    GLfloat mat_emission[] = {rgbaColor[0], rgbaColor[1], rgbaColor[2], rgbaColor[3]};
    GLfloat mat_shininess = 90.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, mat_shininess);
}

/**
 * 而对于 LightPlanet 类来说，属于发光的星球，所以我们不但要设置其光照材质，还要设置其光源位置
 */
void LightPlanet::drawLight()
{
    GLfloat light_position[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat light_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 指定零号光源的位置
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);  // 表示各种光线照射到该材质上，经过很多次反射后追踪遗留在环境中的光线强度
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);  // 漫反射后的光照强度
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); // 镜面反射后的光照强度
}

LightPlanet::LightPlanet(GLfloat radius, GLfloat distance, GLfloat speed, GLfloat selfSpeed,
                         Star * parent, GLfloat rgbClolor[3]) :
    Planet(radius, distance, speed, selfSpeed, parent, rgbClolor)
{
}

