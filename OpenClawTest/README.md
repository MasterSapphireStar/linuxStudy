# OpenClawTest · Interactive 3D Solar System

[中文说明](#中文说明) | [English](#english)

---

## 中文说明

`OpenClawTest` 当前提供了一个基于 **Three.js** 的交互式 3D 太阳系演示页面。

### 功能特点

- 3D 太阳系可视化
- 行星公转与自转动画
- 鼠标拖拽 / 缩放 / 平移视角
- 点击行星查看基础信息
- 支持轨道显隐与视角重置

### 使用方式

直接在浏览器中打开：

- `solar-system.html`

如果浏览器因本地资源策略导致外部纹理加载异常，建议使用本地静态服务器打开，例如：

```bash
python3 -m http.server 8000
```

然后访问：

```text
http://localhost:8000/OpenClawTest/solar-system.html
```

### 适合继续改进的方向

- 增加更明确的交互提示与新手引导
- 补充行星数据来源说明
- 增加移动端适配
- 优化加载失败与纹理降级提示
- 增加 README、截图与演示说明

---

## English

`OpenClawTest` currently contains an interactive **3D solar system demo** built with **Three.js**.

### Features

- Interactive 3D solar system visualization
- Planet orbit and rotation animation
- Mouse-based rotate / zoom / pan controls
- Click planets to inspect basic information
- Orbit visibility toggle and camera reset

### How to run

Open the following file directly in a browser:

- `solar-system.html`

If external textures fail to load because of local browser restrictions, run a lightweight static server instead:

```bash
python3 -m http.server 8000
```

Then open:

```text
http://localhost:8000/OpenClawTest/solar-system.html
```

### Good next improvements

- clearer in-page onboarding and interaction hints
- documented data sources for planets
- better mobile support
- friendlier load-failure fallback messages
- screenshots and demo-oriented repository docs
