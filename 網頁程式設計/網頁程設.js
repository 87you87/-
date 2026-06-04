
$(document).ready(function(){
    // 預設日期為今天
    let today = new Date();

    let yyyy = today.getFullYear();

    let mm =
        String(today.getMonth()+1)
        .padStart(2,'0');

    let dd =
        String(today.getDate())
        .padStart(2,'0');

    $("#date").val(
        `${yyyy}-${mm}-${dd}`
    );
    // 從 localStorage 讀取記帳資料
    let records =
        JSON.parse(localStorage.getItem("records"))
        || [];

    let pieChart;
    let barChart;
    
    // 獲取當前月份數字（1-12）
    let currentMonth = today.getMonth() + 1;
    
    // =========================  
    //網頁載入時執行
    // =========================
    
    // 初始化歷史紀錄過濾選項
    $("#historyYear").val(yyyy);
    $("#historyMonth").val(currentMonth);
    
    loadRecords();
    
    // 設置月曆為當年當月
    $("#calendarYear").val(yyyy);
    $("#calendarMonth").val(currentMonth);
    createCalendar(yyyy, currentMonth);
    
    updateSummary();
    updateCharts();

    // =========================
    // 分頁邏輯
    // =========================
    
    // 隱藏所有主要內容區段
    function hideAllSections(){
        $("#form-section").hide();
        $("#calendar-section").hide();
        $("#chart-section").hide();
        $("#history-section").hide();
    }

    // 處理導航菜單點擊
    $(".nav-links a").click(function(e){
        let target = $(this).attr("href");
        
        // 如果是首頁，不隱藏任何東西
        if(target === "#"){
            return;
        }

        hideAllSections();

        // 顯示對應的區段
        if(target === "#form-section"){
            $("#form-section").show();
        } else if(target === "#calendar-section"){
            $("#calendar-section").show();
            // 當點擊月曆時，重置為當年當月
            $("#calendarYear").val(yyyy);
            $("#calendarMonth").val(currentMonth);
            createCalendar(yyyy, currentMonth);
        } else if(target === "#chart-section"){
            $("#chart-section").show();
            // 當點擊統計分析時，重置為當年並更新圖表
            $("#yearSelect").val(new Date().getFullYear());
            $("#yearSelect").trigger("change");
        } else if(target === "#history-section"){
            $("#history-section").show();
            // 當點擊歷史紀錄時，重置為當年當月
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

    // =========================
    // 月曆年月改變
    // =========================
    
    $(document).on("change", "#calendarYear, #calendarMonth", function(){
        let year = parseInt($("#calendarYear").val());
        let month = parseInt($("#calendarMonth").val());
        createCalendar(year, month);
    });

    // =========================
    // 歷史紀錄篩選和排序
    // =========================
    
    $(document).on("change", "#historyYear, #historyMonth", function(){
        loadRecords();
    });

    $(document).on("click", "#sortAscBtn", function(){
        records.sort((a, b) => new Date(a.date) - new Date(b.date));
        loadRecords();
    });

    $(document).on("click", "#sortDescBtn", function(){
        records.sort((a, b) => new Date(b.date) - new Date(a.date));
        loadRecords();
    });

    // =========================
    // 表單提交
    // =========================    
    $("#expenseForm").submit(function(e){

        e.preventDefault();

        let type =
            $("input[name='type']:checked").val();

        let amount =
            $("#amount").val();

        let date =
            $("#date").val();

        let category =
            $("#category").val();

        let note =
            $("#note").val();

        let data = {

            type:type,
            amount:Number(amount),
            date:date,
            category:category,
            note:note

        };

        // 加入 records
        records.push(data);

        // 存到 localStorage
        localStorage.setItem(
            "records",
            JSON.stringify(records)
        );

        // 重新加載表格（以正確顯示過濾結果）
        loadRecords();

        // 更新統計
        updateSummary();

        // 更新圖表
        updateCharts();

        // 保存類別選項
        let savedCategory = $("#category").val();

        // 清空表單
        $("#expenseForm")[0].reset();

        // 恢復類別選項
        $("#category").val(savedCategory);

        // 日期重置為今天
        $("#date").val(
            `${yyyy}-${mm}-${dd}`
        );

        // 顯示成功提示
        alert("✓ 已新增記帳資料");

    });

    // =========================
    // 刪除資料
    // =========================

    $(document).on(
    "click",
    ".deleteBtn",
    function(){

        let recordDate =
            $(this)
            .closest("tr")
            .attr("data-date");

        // 查找並刪除對應日期的記錄
        let deleteIndex = records.findIndex(r => r.date === recordDate);
        
        if(deleteIndex !== -1){
            records.splice(deleteIndex, 1);
        }

        localStorage.setItem(
            "records",
            JSON.stringify(records)
        );

        loadRecords();

        updateSummary();

        updateCharts();

    }
);

    

    // =========================
    // 新增表格列
    // =========================    
    function addRow(record){

    let row = `

    <tr data-date="${record.date}">

        <td>${record.date}</td>

        <td>${record.type}</td>

        <td>${record.category}</td>

        <td>${record.amount}</td>

        <td>${record.note}</td>

        <td>

            <button class="deleteBtn">

                刪除

            </button>

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

        // 年份過濾
        let selectedYear = $("#historyYear").val();
        if(selectedYear){
            filteredRecords = filteredRecords.filter(record => {
                let recordYear = new Date(record.date).getFullYear().toString();
                return recordYear === selectedYear;
            });
        }

        // 月份過濾
        let selectedMonth = $("#historyMonth").val();
        if(selectedMonth){
            filteredRecords = filteredRecords.filter(record => {
                let recordMonth = (new Date(record.date).getMonth() + 1).toString();
                return recordMonth === selectedMonth;
            });
        }

        $("#historyTable").empty();

        filteredRecords.forEach(record=>{
            addRow(record);
        });
    }
    // =========================
    // 更新收入支出統計
    // =========================    
    function updateSummary(){

    let income=0;
    let expense=0;

    records.forEach(record=>{

        if(record.type==="收入")
            income+=record.amount;

        else
            expense+=record.amount;

    });

    $("#incomeTotal").text("$"+income);

    $("#expenseTotal").text("$"+expense);

    $("#profitTotal").text(
        "$"+(income-expense)
    );

    }
    // =========================
    //產生月曆
    // =========================
    function createCalendar(year,month){

    $("#calendar").empty();

    // 注意：month 參數是 1-12，需要轉換為 0-11
    let daysInMonth = new Date(year, month, 0).getDate();

    // 產生日期格子
    for(let i = 1; i <= daysInMonth; i++){
        $("#calendar").append(`

        <div class="day"
            data-date="${year}-${String(month).padStart(2,'0')}-${String(i).padStart(2,'0')}">

            ${i}

        </div>

        `);
    }

    // 點擊日期顯示當天記帳紀錄
    $(document).on(
        "click",
        ".day",
        function(){

            let day =
                $(this).text();

            let result = "";

            records.forEach(r=>{

                let d =
                    new Date(r.date)
                    .getDate();

                if(d == day){

                    result +=
                    `${r.category}
                    ${r.amount}\n`;

                }

            });

            if(result==="")
                result="當天沒有紀錄";

            alert(result);

        }
    );


    }

    // =========================
    // 更新圖表
    // =========================    
    function updateCharts(){

    let incomeMonth =
        Array(12).fill(0);

    let expenseMonth =
        Array(12).fill(0);

    let categoryData = {

        "飲食":0,
        "交通":0,
        "娛樂":0,
        "購物":0,
        "學習":0,
        "其他":0

    };
    records.forEach(record=>{

        let date =
            new Date(record.date);

        let month =
            date.getMonth();

        if(record.type==="收入"){

            incomeMonth[month]+=record.amount;

        }
        else{

            expenseMonth[month]+=record.amount;

            categoryData[
                record.category
            ] += record.amount;

        }

    });

    let profitMonth =
        incomeMonth.map(
            (v,i)=>v-expenseMonth[i]
        );

    let maxValue =
        Math.max(
            ...incomeMonth,
            ...expenseMonth,
            1000
        );

    if(pieChart)
        pieChart.destroy();

    if(barChart)
        barChart.destroy();

    pieChart =
        new Chart(
            document.getElementById("pieChart"),
            {
                type:"pie",

                data:{

                    labels:[
                        "飲食",
                        "交通",
                        "娛樂",
                        "購物",
                        "學習",
                        "其他"
                    ],

                    datasets:[{

                        data:[

                            categoryData["飲食"],
                            categoryData["交通"],
                            categoryData["娛樂"],
                            categoryData["購物"],
                            categoryData["學習"],
                            categoryData["其他"]

                        ]

                    }]
                }
            }
        );

    barChart =
        new Chart(
            document.getElementById("barChart"),
            {
                type:"bar",

                data:{

                    labels:[
                        "1月","2月","3月","4月",
                        "5月","6月","7月","8月",
                        "9月","10月","11月","12月"
                    ],

                    datasets:[

                    {
                        label:"收入",
                        data:incomeMonth
                    },

                    {
                        label:"支出",
                        data:expenseMonth
                    },

                    {
                        label:"盈虧",
                        data:profitMonth
                    }
                    ]

                },

                options:{

                    responsive:true,

                    scales:{

                        y:{

                            beginAtZero:true,

                            suggestedMax:
                                Math.ceil(maxValue/1000)
                                *1000

                        }

                    }

                }

            }
        );

    }
    // 年份選擇改變時更新圖表

    $("#yearSelect").change(function(){
        let selectedYear = Number($("#yearSelect").val());
        
        // 根據選定年份過濾記錄
        let filteredRecords = records.filter(record => {
            let date = new Date(record.date);
            return date.getFullYear() === selectedYear;
        });

        // 重新計算圖表數據
        let incomeMonth = Array(12).fill(0);
        let expenseMonth = Array(12).fill(0);
        let categoryData = {
            "飲食":0,
            "交通":0,
            "娛樂":0,
            "購物":0,
            "學習":0,
            "其他":0
        };

        filteredRecords.forEach(record => {
            let date = new Date(record.date);
            let month = date.getMonth();

            if(record.type === "收入"){
                incomeMonth[month] += record.amount;
            } else {
                expenseMonth[month] += record.amount;
                categoryData[record.category] += record.amount;
            }
        });

        // 更新圖表
        let profitMonth = incomeMonth.map((v, i) => v - expenseMonth[i]);
        let maxValue = Math.max(...incomeMonth, ...expenseMonth, 1000);

        if(pieChart) pieChart.destroy();
        if(barChart) barChart.destroy();

        pieChart = new Chart(
            document.getElementById("pieChart"),
            {
                type:"pie",
                data:{
                    labels:["飲食","交通","娛樂","購物","學習","其他"],
                    datasets:[{
                        data:[
                            categoryData["飲食"],
                            categoryData["交通"],
                            categoryData["娛樂"],
                            categoryData["購物"],
                            categoryData["學習"],
                            categoryData["其他"]
                        ]
                    }]
                }
            }
        );

        barChart = new Chart(
            document.getElementById("barChart"),
            {
                type:"bar",
                data:{
                    labels:["1月","2月","3月","4月","5月","6月","7月","8月","9月","10月","11月","12月"],
                    datasets:[
                        {
                            label:"收入",
                            data:incomeMonth
                        },
                        {
                            label:"支出",
                            data:expenseMonth
                        },
                        {
                            label:"盈虧",
                            data:profitMonth
                        }
                    ]
                },
                options:{
                    responsive:true,
                    scales:{
                        y:{
                            beginAtZero:true,
                            suggestedMax: Math.ceil(maxValue/1000)*1000
                        }
                    }
                }
            }
        );
    });

    // =========================
    // Header 粘性導航
    // =========================
    let lastScrollTop = 0;
    let headerHidden = false;

    $(window).scroll(function(){
        let currentScroll = $(this).scrollTop();

        if(currentScroll > lastScrollTop && currentScroll > 100){
            // 向下滑
            if(!headerHidden){
                $("header").removeClass("show");
                headerHidden = true;
            }
        } else {
            // 向上滑
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