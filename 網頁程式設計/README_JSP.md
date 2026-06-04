# 智慧記帳管理系統 - JSP 版本

## 📋 項目概述

此項目已從純前端應用升級為支援 JSP 後端的完整 Web 應用。

## 🛠️ 技術棧

| 技術 | 版本 | 用途 |
|-----|------|------|
| **HTML** | 5 | 頁面結構和語義標記 |
| **CSS** | 3 | 樣式和布局設計 |
| **JavaScript** | ES6+ | 前端邏輯和交互 |
| **jQuery** | 3.7.1 | DOM 操作和事件處理 |
| **JSP** | 2.3 | 動態頁面生成 |
| **Servlet** | 4.0 | 業務邏輯處理 |
| **JSTL** | 1.2 | JSP 標籤庫 |

## 📁 文件結構

```
網頁程式設計/
├── 網頁程設.html              主頁面（HTML5）
├── 網頁程設.css               樣式表（CSS3）
├── 網頁程設.js                前端邏輯（JavaScript + jQuery）
├── index.jsp                  JSP 首頁
├── WEB-INF/
│   ├── web.xml               部署描述符（Deployment Descriptor）
│   ├── jsp/
│   │   └── expenseList.jsp    記帳列表頁面（JSP）
│   ├── classes/              編譯後的 Servlet 類
│   └── lib/                   依賴的 JAR 庫
└── README_JSP.md             本文件
```

## 🚀 Eclipse 佈署指南

### 前置條件
- Eclipse IDE for Enterprise Java Developers
- Apache Tomcat 9.0 或更高版本
- JDK 8 或更高版本

### 步驟

1. **在 Eclipse 中建立 Dynamic Web Project**
   ```
   File → New → Dynamic Web Project
   Project name: 網頁程式設計
   Target runtime: Apache Tomcat v9.0
   Dynamic web module version: 4.0
   ```

2. **複製項目文件**
   - 將 HTML、CSS、JS 文件複製到項目根目錄
   - 將 `WEB-INF` 文件夾複製到 WebContent 目錄

3. **配置 Tomcat**
   ```
   Window → Preferences → Server → Runtime Environments
   → Add → Apache Tomcat v9.0
   ```

4. **建立 Servlet（如需要）**
   ```
   src/com/expense/servlet/ExpenseServlet.java
   ```

5. **部署到 Tomcat**
   ```
   右擊項目 → Run on Server → Apache Tomcat v9.0
   ```

6. **訪問應用**
   ```
   http://localhost:8080/網頁程式設計/index.jsp
   ```

## 📝 主要文件說明

### web.xml
部署描述符，定義了：
- Servlet 的映射
- 歡迎文件清單
- Session 配置
- 安全設置

### index.jsp
JSP 首頁，展示：
- 項目信息
- 技術棧列表
- Eclipse 佈署指南

### expenseList.jsp
記帳列表頁面，使用：
- JSTL `<c:forEach>` 迴圈遍歷數據
- JSTL `<c:if>` 條件判斷
- JSP Expression Language (EL)

## 🔄 工作流程

```
前端 (HTML/CSS/JavaScript)
        ↓
jQuery AJAX 請求
        ↓
Servlet 處理請求
        ↓
業務邏輯（未來連接數據庫）
        ↓
JSP 生成動態頁面
        ↓
JSON 響應或 HTML 返回
```

## 💾 數據存儲

**當前實現：** `localStorage`（瀏覽器本地存儲）

**未來改進：** 
- MySQL 數據庫
- JDBC 或 JPA 連接
- Hibernate ORM 框架

## 🔒 安全考慮

1. **輸入驗證**
   - 服務器端驗證所有輸入
   - 防止 SQL 注入
   - 防止 XSS 攻擊

2. **認證和授權**
   - 用戶登錄系統
   - Session 管理
   - 角色基礎訪問控制（RBAC）

3. **數據加密**
   - HTTPS 加密傳輸
   - 密碼加密存儲

## 📈 擴展建議

### 短期
- [ ] 建立 MySQL 數據庫
- [ ] 實現 CRUD 操作的 Servlet
- [ ] 添加用戶認證

### 中期
- [ ] 構建 REST API
- [ ] 實現分頁和搜索功能
- [ ] 添加數據導出功能

### 長期
- [ ] 微服務架構
- [ ] 前後端完全分離（React/Angular + Spring Boot）
- [ ] 移動應用適配

## 🐛 常見問題

**Q: 無法找到 JSP 文件**
A: 檢查 WEB-INF 的位置是否正確，應在 WebContent/WEB-INF 下

**Q: 404 錯誤**
A: 檢查 web.xml 的 servlet-mapping 配置，路徑是否正確

**Q: Servlet 編譯錯誤**
A: 確保 JDK 版本與項目兼容，清理並重建項目

## 📚 參考資料

- [Oracle JSP Documentation](https://docs.oracle.com/cd/E24329_01/web.1211/e24589/toc.htm)
- [Apache Tomcat Documentation](https://tomcat.apache.org/tomcat-9.0-doc/)
- [JSTL Tag Reference](https://tomcat.apache.org/tomcat-9.0-doc/jstl/reference.html)

## ✅ 檢查清單

- [x] HTML 5 結構
- [x] CSS 3 樣式
- [x] JavaScript 邏輯
- [x] jQuery 庫
- [x] JSP 動態頁面
- [x] web.xml 配置
- [x] Servlet 架構
- [x] JSTL 標籤庫
- [ ] 數據庫連接
- [ ] 用戶認證

---

**版本：** 1.0  
**最後更新：** 2026年6月4日  
**作者：** 網頁程式設計課程
