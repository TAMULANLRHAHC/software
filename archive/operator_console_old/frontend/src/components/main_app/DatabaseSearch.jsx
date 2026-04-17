import {ScrollArea, ScrollBar} from "@/components/ui/scroll-area.jsx"
import { Input } from "@/components/ui/input.jsx"
import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
} from "@/components/ui/table.jsx"
import {useEffect, useRef, useState} from "react";
import {socket} from "@/other/socket.js";
import {Label} from "@/components/ui/label.jsx";
import {ChartContainer, ChartTooltip, ChartTooltipContent} from "@/components/ui/chart.jsx";
import {Line, LineChart} from "recharts";
import {
    transformFlatJsonToNested
} from "@/other/helperfunctions.js";

let currently_being_edited =
    {
        tag_name: "none",
        tag_field: "none",
        desired_value: "none"
    };

export function DatabaseSearch() {
    const [rows, setRows] = useState("");

    //tag name should always be first, and graph should always be last.
    let column_names = {"tag": {}, "value": {"className": "w-[600px]"}, "units": {"className": "w-[600px]"}};
    const [columns, setColumns] = useState("")

    //show tags whose fields are not being listed currently
    let show_tags_with_no_matching_columns = false;

    //Fires when the component is first rendered.
    useEffect(() => {
        //send empty query to recieve all data available
        socket.emit("database_viewer_subscribe_to_query", "");

        //generate set of columns & set them
        let columns_generated = generateColumns(column_names);
        setColumns(columns_generated);

        return ()=>{
            socket.emit("database_viewer_unsubscribe_from_query", "")
        }
    }, []);

    //When server sends database query results
    useEffect(() => {
        socket.on("database_viewer_query_results", (data) => {
            let new_rows;

            let query_results = data["query_results"]
            let query = data["query"]

            //if there are query results
            if (query_results !== null) {
                new_rows = generateRows(query, query_results);
                //if there are no query results
            } else {
                new_rows = [<TableCell>No data found.</TableCell>];
            }

            setRows(new_rows);

            //ensure we stop listening for event once we undock the search table
            return (
                socket.off("database_query_results")
            );
        })
    }, [rows]);

    //Send search bar query to server
    const handleSearchChange = (event) => {
        socket.emit("database_viewer_subscribe_to_query", event.target.value);
    }

    //Takes the query data formatted by the server and generates rows to add to the table
    //Server keeps querying & sending incase a new tag is created
    function generateRows(query, query_results) {

        //transform into nested JSON
        let transformed_results = transformFlatJsonToNested(query_results)

        let rows = []
        let row_num = 0;
        //for each entry in the flattened project map structure
        for (const [tag_name, tag_field_json] of Object.entries(transformed_results)) {

            //first, check if tag has no matching columns (if option is set)
            if (!show_tags_with_no_matching_columns) {
                // convert keys to lowercase for comparison
                const keysColumnNames = Object.keys(column_names).map(key => key.toLowerCase());
                const keysTagFields = Object.keys(tag_field_json).map(key => key.toLowerCase());

                const hasSharedKeys = keysColumnNames.some(key => keysTagFields.includes(key));

                //if no keys match
                if (!hasSharedKeys) {
                    //skip displaying this tag
                    continue;
                }
            }

            //cells
            let cells = [];

            //for each column name currently shown
            for (const [column_name, column_name_options] of Object.entries(column_names)) {
                //check if column_name is "Tag Name"
                if (column_name === "tag") {

                    //create highlighting effect
                    const [first, ...rest] = tag_name.split(query)

                    const index = tag_name.toLowerCase().indexOf(query.toLowerCase())

                    if (index !== -1) {
                        let front = tag_name.slice(0, index)
                        let middle = tag_name.slice(index, index+query.length)
                        let end = tag_name.slice(index+query.length)

                        //add it
                        cells.push(
                            <TableCell>
                                <div className="h-full w-full flex items-center normal-case">
                                    <Label id={query+"_0"} >{front}</Label>
                                    <Label id={query+"_query"} className="bg-yellow-300/40 rounded">{middle}</Label>
                                    <Label id={query+"_1"}>{end}</Label>
                                </div>

                            </TableCell>
                        )
                    } else {
                        cells.push(
                            <TableCell>
                                <Label id={query+"_0"} >{tag_name}</Label>
                            </TableCell>
                        )
                    }

                    continue;
                }

                //check if column name is graph for graph
                if (column_name === "graph") {
                    let series_name = column_name_options.series
                    let domain = column_name_options.domain

                    const newDataPoint = {}
                    //formats y value (the data value whose key matches dataKey)
                    newDataPoint[series_name] = tag_field_json[series_name] || 0

                    //add it
                    cells.push(
                        <TableCell>
                            <DatabaseSearchGraph
                                key={tag_name}
                                className="z-1"
                                newDataPoint={newDataPoint}
                                domain={domain}
                                dataKey={series_name}
                            />
                        </TableCell>
                    )
                    continue;
                }


                /// SEE IF TAG FIELD IS A COLUMN WE ARE CURRENTLY DISPLAYING ///
                //check if current column name is an existing tag field for this tag (ensure lower case)
                let lowercase_column_name = column_name.toLowerCase()

                //IT IS
                if (lowercase_column_name in tag_field_json) {
                    //if the tag field is a current column name add it to the cell list

                    //THE VALUE TO UPDATE
                    let value = tag_field_json[lowercase_column_name];

                    ///UPDATE WITH VALUE USER IS TRYING TO EDIT
                    //if this field is currently being edited, write the desired value
                    if ((tag_name === currently_being_edited.tag_name) && (column_name.toLowerCase() === currently_being_edited.tag_field)) {
                        cells.push(
                            <TableCell className="w-[200px]">
                                <DatabaseEditableField tag_name={tag_name} tag_field={lowercase_column_name}
                                                       val={currently_being_edited.desired_value}/>

                            </TableCell>
                        )
                        //UPDATE WITH VALUE FROM SERVER
                    } else {
                        cells.push(
                            <TableCell className="w-[200px]">
                                <DatabaseEditableField tag_name={tag_name} tag_field={lowercase_column_name}
                                                       val={value}/>
                            </TableCell>
                        )
                    }
                    //IT IS NOT
                } else {
                    //if the current column name does not exist as a tag field, push an empty cell
                    cells.push(
                        <TableCell>-</TableCell>
                    )
                }
            }

            //after we have built all cells, add a row. the last cell is always the step chart
            rows.push(
                <TableRow key={row_num}>
                    {cells}
                </TableRow>
            )
            row_num++
        }

        return rows;
    }

    //takes the column names and generates them in order listed (with optional width if given)
    function generateColumns(columns_name_dict) {
        let columns_generated = [];
        let column_num = 0;

        //iterate through each column name given and its options
        for (const [column_name, column_name_options] of Object.entries(columns_name_dict)) {
            //get optional class name if given
            if ("className" in column_name_options) {
                let classNameString = column_name_options["className"];

                columns_generated.push(
                    <TableHead key={column_num} className="${classNameString}">{column_name}</TableHead>
                )
                //if no class name is given
            } else {
                columns_generated.push(
                    <TableHead key={column_num} >{column_name}</TableHead>
                )
            }
            column_num++;
        }

        return columns_generated;
    }

    return(
        <div className="p-4 h-full w-full">
            <div className="grid grid-cols-1 h-full w-full relative ">
                <Input className="p-4 absolute " placeholder="Search for a tag"
                       onChange={handleSearchChange} defaultValue={""}/>
                {/* Quick fix calculation on the scroll bar below to calculate optimum height - so it doesnt clip into the input */}
                <ScrollArea className="p-2 absolute top-0 inset-y-[50px] h-[calc(100vh-130px)]">
                    <Table className="">
                        <TableHeader className="bg-neutral-900 z-10">
                            <TableRow>
                                {columns}
                            </TableRow>
                        </TableHeader>
                        <TableBody>
                            {rows}
                        </TableBody>
                    </Table>
                    <ScrollBar orientation="horizontal" />
                    <ScrollBar orientation="vertical" />
                </ScrollArea>
            </div>
        </div>
    );
};

function DatabaseEditableField(props) {

    let tag_name = props.tag_name;
    let tag_field = props.tag_field;
    let database_value = props.val;
    let selected = false;

    const handleKeyPress = (event) => {
        if (event.key === 'Enter') {
            let data = {"tag_path": tag_name + "." + tag_field, "desired_value": event.target.value}
            socket.emit("update_tag", data);
            event.target.blur()
        }
    };

    const onSelection = (event) => {
        currently_being_edited.tag_name = tag_name;
        currently_being_edited.tag_field = tag_field;
        currently_being_edited.desired_value = event.currentTarget.value;
    }

    const onChange = (event) => {
        currently_being_edited.tag_name = tag_name;
        currently_being_edited.tag_field = tag_field;
        currently_being_edited.desired_value = event.currentTarget.value;
    }

    // eslint-disable-next-line no-unused-vars
    const unSelected = (event) => {
        currently_being_edited.tag_name = "none";
        currently_being_edited.tag_field = "none";
        currently_being_edited.desired_value = "none";
    }

    return (
        <Input className="bg-zinc-950"
               type="text"
               value={database_value}
               onSelect={onSelection}
               onBlur={unSelected}
               onChange={onChange}
               onKeyDown={handleKeyPress}
        />
    );
}

function DatabaseSearchGraph({newDataPoint, domain, dataKey}) {
    const [chartData, setChartData] = useState([]);
    const lastUpdateTimeRef = useRef(0); // Track the last update timestamp

    useEffect(() => {
        if (newDataPoint !== undefined) {
            const now = Date.now() / 1000; // Current time in seconds
            const dataPoint = { ...newDataPoint, time: now}; //append time

            setChartData((prevData) => {
                // Add the new data point
                const updatedData = [...prevData, dataPoint];
                // Keep only data within the domain (in seconds)
                return updatedData.filter((point) => now - point.time <= domain);
            });
        }
    }, [newDataPoint]);

    let chartConfig = {};
    chartConfig[dataKey] = {
        label: dataKey,
        color: "#93a2a6"
    }

    return (
        <ChartContainer className="h-[40px] w-[200px]" config={chartConfig}>
            <LineChart
                accessibilityLayer
                data={chartData}
                margin={{
                    left: 0,
                    right: 0,
                }}
            >
                <ChartTooltip cursor={false} content={<ChartTooltipContent hideLabel />} />
                <Line dataKey={dataKey}
                      type="linear"
                      strokeWidth={2}
                      stroke={chartConfig[dataKey].color}
                      dot={false} />
            </LineChart>
        </ChartContainer>
    )
}