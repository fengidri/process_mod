IntervalFun = AllProc;
var Pid = 1;

$("#table_list_all_proc tbody tr").each(
    function()
    { 
        alert(this)

        $(this).click(
            function()
            { 
                $("#list_all_proc").hide();
                $("#Canvas").show();

                alert(this)
                Pid = $(this).children('td').eq(0).text();
                alert(Pid)
                IntervalFun = OneProc;

            }
        )
    }
)

setInterval(
    function()
    { 
                //OneProc();
                IntervalFun();

    },1000);

function AllProc()
{ 
    var TrLen = $("#table_list_all_proc tbody tr").length;
    $.get("/AllProc",function(result)
          {
              obj =jQuery.parseJSON(result);
              var i;
              for( i in obj.proc)
                  { 
                      if (i<TrLen)
                          { 
                             tr =  $("#table_list_all_proc tbody tr").eq(i);
                             tr.children('td').eq(0).html(obj.proc[i].pid);
                             tr.children('td').eq(1).html(obj.proc[i].name);
                             tr.children('td').eq(2).html(obj.proc[i].state);
                             tr.children('td').eq(3).html(obj.proc[i].rss);
                             tr.children('td').eq(4).html(obj.proc[i].cpu);
                             tr.children('td').eq(5).html(obj.proc[i].pri);
                             tr.children('td').eq(6).html(obj.proc[i].ni);
                             tr.children('td').eq(7).html(obj.proc[i].time);
                          }
                          else
                         { 
                             var newRow = "<tr><td>" 
                             + obj.proc[i].pid  + "</td><td>" 
                             + obj.proc[i].name + "</td><td>" 
                             + obj.proc[i].state + "</td><td>"
                             + obj.proc[i].rss  + "</td><td>"
                             + obj.proc[i].cpu + "</td><td>"
                             + obj.proc[i].pri + "</td><td>"
                             + obj.proc[i].ni + "</td><td>"
                             + obj.proc[i].time + "</td><td>"

                             $("#table_list_all_proc tr:last").after(newRow);
                             $("#table_list_all_proc tr:last").click(
                                 function()
                                 { 
                                     $("#list_all_proc").hide();
                                     $("#Canvas").show();

                                     Pid = $(this).children('td').eq(0).text();
                                     IntervalFun = OneProc;

                                 })
                         }
                  };
            for (i ; i<TrLen; i++)
                { 
                  tr =  $("#table_list_all_proc tbody tr").eq(i).remove();
                }
          });

}
function OneProc()
{ 
    $.get("/OneProc/"  +  Pid,function(result)
            {
                obj =jQuery.parseJSON(result);
                draw_data_array.push_array(obj.cpuarray);
                draw_chart();
            }
            );

}



/**
 *test
 */
UNIT=5;
THE_CANVAS_WIDTH=60 * UNIT;
THE_CANVAS_HEIGHT=30 * UNIT;

//^^^^^^^^^^^^^^^^^^^^^^^^[START:SqQueue]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
draw_data_array = [];
draw_data_array.front = 0;
draw_data_array.rear = 0;
draw_data_array.push = function(element)
{ 
    if ((this.rear  +  1) % THE_CANVAS_WIDTH 
            == this.front)
    { 
        this[this.rear] = element;
        this.front = (this.front  + 1)  % 
            THE_CANVAS_WIDTH;
        this.rear = (this.rear  + 1) % 
            THE_CANVAS_HEIGHT;
        return 0;
    }
    else
    { 
        this[this.rear] = element;
        this.rear =  (this.rear  + 1) % 
            THE_CANVAS_WIDTH; 
        return 1;

    }

}
draw_data_array.push_array = function(array)
{ 
    for (i in array)
    { 
        this.push(array[i]);

    }

}
/**
 * 从队尾开始,偏移了offset值
 */
draw_data_array.offset = function(offset)
{ 
    return this[(this.rear  - offset  - 1  + THE_CANVAS_WIDTH) % THE_CANVAS_WIDTH];

}

function draw_chart()
{
    var ii;
    ii = 0;
    canvas_obj.clearRect(0, 0, THE_CANVAS_WIDTH, THE_CANVAS_HEIGHT);
    canvas_obj.beginPath();
    canvas_obj.moveTo(
            THE_CANVAS_WIDTH - ii*UNIT, 
            THE_CANVAS_HEIGHT* (1 - draw_data_array.offset(ii)));

    for (ii = 1; ii<  THE_CANVAS_WIDTH; ii++)
    { 
        canvas_obj.lineTo(
                THE_CANVAS_WIDTH - ii*UNIT, 
                THE_CANVAS_HEIGHT* (1 - draw_data_array.offset(ii)));
    }

    canvas_obj.stroke();
}
