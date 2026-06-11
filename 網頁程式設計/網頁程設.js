$(document).ready(function(){
    // =========================
    // 初始設定
    // =========================
    let today = new Date();
    let yyyy = today.getFullYear();
    let mm = String(today.getMonth() + 1).padStart(2, '0');
    let dd = String(today.getDate()).padStart(2, '0');
    let currentMonth = today.getMonth() + 1;

    let pieChart;
    let barChart;
    let editingId = null;

    // 從 localStorage 讀取記帳資料
    let records = JSON.parse(localStorage.getItem("records")) || [];

    // 幫舊資料補上 id，避免同一天多筆資料時刪除或修改錯誤
    records = records.map(function(record){
        return {
            id: record.id || createId(),
            type: record.type || "支出",
            amount: Number(record.amount) || 0,
            date: record.date || `${yyyy}-${mm}-${dd}`,
            category: record.category || "其他",
            note: record.note || ""
        };
    });
    saveRecords();

    // 預設的支出和收入類別
        const expenseCategories = [
            "飲食",
            "交通",
            "娛樂",
            "購物",
            "學習",
            "其他"
        ];

        const incomeCategories = [
            "薪資",
            "獎金",
            "投資",
            "兼職",
            "獎學金",
            "其他收入"
        ];


    // 預設日期為今天
    $("#date").val(`${yyyy}-${mm}-${dd}`);

    updateCategoryOptions();

    // 初始化篩選與月曆
    $("#historyYear").val(yyyy);
    $("#historyMonth").val(currentMonth);
    $("#calendarYear").val(yyyy);
    $("#calendarMonth").val(currentMonth);

    loadRecords();
    createCalendar(yyyy, currentMonth);
    updateSummary();
    updateCharts();

    // =========================
    // 工具函式
    // =========================
    
    function createId(){
        return Date.now().toString() + "_" + Math.random().toString(36).substring(2, 10);
    }

    function saveRecords(){
        localStorage.setItem("records", JSON.stringify(records));
    }

    function escapeHtml(text){
        return String(text)
            .replaceAll("&", "&amp;")
            .replaceAll("<", "&lt;")
            .replaceAll(">", "&gt;")
            .replaceAll('"', "&quot;")
            .replaceAll("'", "&#039;");
    }

    function hideAllSections(){
        $("#form-section").hide();
        $("#calendar-section").hide();
        $("#chart-section").hide();
        $("#history-section").hide();
    }

    function resetFormToAddMode(){
        editingId = null;
        $("#expenseForm h2").text("新增記帳資料");
        $("#expenseForm button[type='submit']").text("新增資料");
        $("#cancelEditBtn").remove();
        $("#expenseForm")[0].reset();
        $("input[name='type'][value='支出']").prop("checked", true);
        
        updateCategoryOptions();
        
        $("#date").val(`${yyyy}-${mm}-${dd}`);
    }

    // 根據選擇的類型更新類別選項

    function updateCategoryOptions(){

        let selectedType =
            $("input[name='type']:checked").val();

        let categories =
            selectedType === "收入"
            ? incomeCategories
            : expenseCategories;

        $("#category").empty();

        categories.forEach(function(category){

            $("#category").append(
                `<option value="${category}">
                    ${category}
                </option>`
            );

        });
    }

    function refreshAll(){
        saveRecords();
        loadRecords();
        updateSummary();
        updateCharts();

        let calendarYear = parseInt($("#calendarYear").val());
        let calendarMonth = parseInt($("#calendarMonth").val());
        createCalendar(calendarYear, calendarMonth);
    }

    // =========================
    // 分頁邏輯
    // =========================
    $(".nav-links a").click(function(e){
        let target = $(this).attr("href");

        if(target === "#"){
            return;
        }

        hideAllSections();

        if(target === "#form-section"){
            $("#form-section").show();
        } else if(target === "#calendar-section"){
            $("#calendar-section").show();
            $("#calendarYear").val(yyyy);
            $("#calendarMonth").val(currentMonth);
            createCalendar(yyyy, currentMonth);
        } else if(target === "#chart-section"){
            $("#chart-section").show();
            $("#yearSelect").val(new Date().getFullYear());
            updateCharts();
        } else if(target === "#history-section"){
            $("#history-section").show();
            $("#historyYear").val(yyyy);
            $("#historyMonth").val(currentMonth);
            loadRecords();
        }
    });

    // =========================
    // 表單開關
    // =========================
    $(document).on("click", "#toggleFormBtn", function(e){
        e.preventDefault();
        let formSection = $("#form-section");

        if(formSection.is(":hidden")){
            hideAllSections();
            formSection.show();
        } else {
            formSection.hide();
        }
    });

    // 類型改變時更新類別選項
    $(document).on(
        "change",
        "input[name='type']",
        function(){

            updateCategoryOptions();

        }
    );
    // =========================
    // 月曆年月改變
    // =========================
    $(document).on("change", "#calendarYear, #calendarMonth", function(){
        let year = parseInt($("#calendarYear").val());
        let month = parseInt($("#calendarMonth").val());
        createCalendar(year, month);
    });

    // 點擊月曆日期：切換表單日期並顯示當天紀錄
    $(document).on("click", ".day", function(){
        let selectedDate = $(this).attr("data-date");
        $("#date").val(selectedDate);

        let dayRecords = records.filter(function(record){
            return record.date === selectedDate;
        });

        let result = dayRecords.map(function(record){
            return `${record.type}｜${record.category}｜$${record.amount}${record.note ? "｜" + record.note : ""}`;
        }).join("\n");

        if(result === ""){
            result = "當天沒有紀錄，已將表單日期切換到 " + selectedDate;
        } else {
            result = selectedDate + " 的記帳紀錄：\n" + result;
        }

        alert(result);
    });

    // =========================
    // 歷史紀錄篩選和排序
    // =========================
    $(document).on("change", "#historyYear, #historyMonth", function(){
        loadRecords();
    });

    $(document).on("click", "#sortAscBtn", function(){
        records.sort((a, b) => new Date(a.date) - new Date(b.date));
        refreshAll();
    });

    $(document).on("click", "#sortDescBtn", function(){
        records.sort((a, b) => new Date(b.date) - new Date(a.date));
        refreshAll();
    });

    // =========================
    // 表單提交：新增 / 修改
    // =========================
    $("#expenseForm").submit(function(e){
        e.preventDefault();

        let type = $("input[name='type']:checked").val();
        let amount = Number($("#amount").val());
        let date = $("#date").val();
        let category = $("#category").val();
        let note = $("#note").val();

        if(!date){
            alert("請選擇日期");
            return;
        }

        if(!amount || amount <= 0){
            alert("請輸入大於 0 的金額");
            return;
        }

        let data = {
            id: editingId || createId(),
            type: type,
            amount: amount,
            date: date,
            category: category,
            note: note
        };

        if(editingId){
            let index = records.findIndex(record => record.id === editingId);
            if(index !== -1){
                records[index] = data;
                alert("✓ 已修改記帳資料");
            }
        } else {
            $.ajax({
            url:"儲存.jsp",
            type:"POST",
            data:{
                type:type,
                amount:amount,
                date:date,
                category:category,
                note:note
            },

            success:function(response){
                console.log("success");
                console.log(response);

                records.push(data);
                refreshAll();

                alert("資料已送到JSP");
            },

            error:function(xhr,status,error){

                console.log("status=",xhr.status);
                console.log("response=",xhr.responseText);
                console.log("error=",error);

                alert("新增失敗");
            }
        });
            
        }

        refreshAll();
        resetFormToAddMode();
    });

    // =========================
    // 修改資料
    // =========================
    $(document).on("click", ".editBtn", function(){
        let recordId = String($(this).closest("tr").attr("data-id"));
        let record = records.find(r => String(r.id) === recordId);

        if(!record){
            alert("找不到這筆資料，請重新整理頁面後再試一次");
            return;
        }

        editingId = record.id;

        $("input[name='type'][value='" + record.type + "']").prop("checked", true);
        
        updateCategoryOptions();

        $("#amount").val(record.amount);
        $("#date").val(record.date);
        $("#category").val(record.category);
        $("#note").val(record.note);

        $("#expenseForm h2").text("修改記帳資料");
        $("#expenseForm button[type='submit']").text("儲存修改");

        if($("#cancelEditBtn").length === 0){
            $(".form-buttons").append(`<button type="button" id="cancelEditBtn">取消修改</button>`);
        }

        hideAllSections();
        $("#form-section").show();
        window.scrollTo({ top: 0, behavior: "smooth" });
    });

    // 取消修改
    $(document).on("click", "#cancelEditBtn", function(){
        resetFormToAddMode();
    });

    // =========================
    // 刪除資料
    // =========================
    $(document).on("click", ".deleteBtn", function(){
        let recordId = String($(this).closest("tr").attr("data-id"));

        if(!confirm("確認刪除此記錄？")){
            return;
        }

        records = records.filter(record => String(record.id) !== recordId);

        if(String(editingId) === recordId){
            resetFormToAddMode();
        }

        refreshAll();
    });

    // =========================
    // 新增表格列
    // =========================
    function addRow(record){
        let row = `
            <tr data-id="${escapeHtml(record.id)}" data-date="${escapeHtml(record.date)}">
                <td>${escapeHtml(record.date)}</td>
                <td>${escapeHtml(record.type)}</td>
                <td>${escapeHtml(record.category)}</td>
                <td>${escapeHtml(record.amount)}</td>
                <td>${escapeHtml(record.note)}</td>
                <td>
                    <button type="button" class="editBtn">修改</button>
                    <button type="button" class="deleteBtn">刪除</button>
                </td>
            </tr>
        `;

        $("#historyTable").append(row);
    }

    // =========================
    // 載入歷史記帳資料
    // =========================
    function loadRecords(){
        let filteredRecords = records.slice();

        let selectedYear = $("#historyYear").val();
        if(selectedYear){
            filteredRecords = filteredRecords.filter(record => {
                let recordYear = new Date(record.date).getFullYear().toString();
                return recordYear === selectedYear;
            });
        }

        let selectedMonth = $("#historyMonth").val();
        if(selectedMonth){
            filteredRecords = filteredRecords.filter(record => {
                let recordMonth = (new Date(record.date).getMonth() + 1).toString();
                return recordMonth === selectedMonth;
            });
        }

        $("#historyTable").empty();

        filteredRecords.forEach(record => {
            addRow(record);
        });
    }

    // =========================
    // 更新收入支出統計
    // =========================
    function updateSummary(){
        let income = 0;
        let expense = 0;

        records.forEach(record => {
            if(record.type === "收入"){
                income += record.amount;
            } else {
                expense += record.amount;
            }
        });

        $("#incomeTotal").text("$" + income);
        $("#expenseTotal").text("$" + expense);
        $("#profitTotal").text("$" + (income - expense));
    }

    // =========================
    // 產生月曆
    // =========================
    function createCalendar(year, month){
        $("#calendar").empty();

        let daysInMonth = new Date(year, month, 0).getDate();

        for(let i = 1; i <= daysInMonth; i++){
            let dateString = `${year}-${String(month).padStart(2, '0')}-${String(i).padStart(2, '0')}`;
            let hasRecord = records.some(record => record.date === dateString);

            $("#calendar").append(`
                <div class="day ${hasRecord ? "has-record" : ""}" data-date="${dateString}">
                    ${i}
                </div>
            `);
        }
    }

    // =========================
    // 更新圖表
    // =========================
    function updateCharts(){
        let selectedYear = Number($("#yearSelect").val()) || yyyy;
        let chartRecords = records.filter(record => {
            let date = new Date(record.date);
            return date.getFullYear() === selectedYear;
        });

        let incomeMonth = Array(12).fill(0);
        let expenseMonth = Array(12).fill(0);
        let categoryData = {
            "飲食": 0,
            "交通": 0,
            "娛樂": 0,
            "購物": 0,
            "學習": 0,
            "其他": 0
        };

        chartRecords.forEach(record => {
            let date = new Date(record.date);
            let month = date.getMonth();

            if(record.type === "收入"){
                incomeMonth[month] += record.amount;
            } else {
                expenseMonth[month] += record.amount;
                categoryData[record.category] += record.amount;
            }
        });

        let profitMonth = incomeMonth.map((v, i) => v - expenseMonth[i]);
        let maxValue = Math.max(...incomeMonth, ...expenseMonth, 1000);

        if(pieChart){
            pieChart.destroy();
        }

        if(barChart){
            barChart.destroy();
        }

        pieChart = new Chart(document.getElementById("pieChart"), {
            type: "pie",
            data: {
                labels: ["飲食", "交通", "娛樂", "購物", "學習", "其他"],
                datasets: [{
                    data: [
                        categoryData["飲食"],
                        categoryData["交通"],
                        categoryData["娛樂"],
                        categoryData["購物"],
                        categoryData["學習"],
                        categoryData["其他"]
                    ]
                }]
            }
        });

        barChart = new Chart(document.getElementById("barChart"), {
            type: "bar",
            data: {
                labels: ["1月", "2月", "3月", "4月", "5月", "6月", "7月", "8月", "9月", "10月", "11月", "12月"],
                datasets: [
                    {
                        label: "收入",
                        data: incomeMonth
                    },
                    {
                        label: "支出",
                        data: expenseMonth
                    },
                    {
                        label: "盈虧",
                        data: profitMonth
                    }
                ]
            },
            options: {
                responsive: true,
                scales: {
                    y: {
                        beginAtZero: true,
                        suggestedMax: Math.ceil(maxValue / 1000) * 1000
                    }
                }
            }
        });
    }

    // 年份選擇改變時更新圖表
    $("#yearSelect").change(function(){
        updateCharts();
    });

    // =========================
    // Header 粘性導航
    // =========================
    let lastScrollTop = 0;
    let headerHidden = false;

    $(window).scroll(function(){
        let currentScroll = $(this).scrollTop();

        if(currentScroll > lastScrollTop && currentScroll > 100){
            if(!headerHidden){
                $("header").removeClass("show");
                headerHidden = true;
            }
        } else {
            if(headerHidden){
                $("header").addClass("show");
                headerHidden = false;
            }
        }

        lastScrollTop = currentScroll <= 0 ? 0 : currentScroll;
    });

    // 初始顯示 header
    $("header").addClass("show");
});
